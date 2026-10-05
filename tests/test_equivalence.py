#!/usr/bin/env python3
"""Reduced, seeded legacy oracle + event/state trace, using only Python stdlib.

The preserved oracle is NEVER edited. Explicit substitutions affect temporary
copies only: fixed seed, original configuration values, and passive counters.
Run make test; --sanitize additionally checks the modular code with ASan/UBSan.
"""
import argparse
import hashlib
import math
import os
from pathlib import Path
import shlex
import statistics
import subprocess
import tempfile

ROOT=Path(__file__).resolve().parents[1]
LEGACY=ROOT/'legacy/CRSOS_Cylindrical_Concept.c'
HASH='5e60fe09e9ba96bb283747dfb464b1e9d792a6c438b9c75388f2263072fe5b1a'
CC=shlex.split(os.environ.get('CC','cc'))
FLAGS=['-std=c11','-D_POSIX_C_SOURCE=200809L','-fno-fast-math','-ffp-contract=off']
SOURCE_NAMES=['main','lattice','crsos','observables','sampling','output','rng']


def run(command,cwd,ok=True):
    r=subprocess.run([str(x) for x in command],cwd=cwd,text=True,capture_output=True,timeout=45)
    if ok and r.returncode:raise AssertionError(f'{command}\n{r.stdout}\n{r.stderr}')
    return r


def replace_once(text,old,new):
    assert text.count(old)==1,(old,text.count(old))
    return text.replace(old,new,1)


def oracle(config,seed):
    s=LEGACY.read_text()
    replacements={
        'RAN3_SEED = GetOddNum();':f'RAN3_SEED = {seed}U;',
        'maxSamples = 10, maxTime = 2000;':f'maxSamples = {config["samples"]}, maxTime = {config["max-time"]};',
        'Omega = 1, M = 1;':f'Omega = {config["omega"]}, M = {config["delta"]};',
        'Lx = 32768, initLy = 4, maxColumns = 2800;':f'Lx = {config["lx"]}, initLy = {config["initial-ly"]}, maxColumns = {config["max-columns"]};',
        'Ly = initLy;':'Ly = initLy;\n    uint64_t events=0, depositions=0, duplications=0, diffusion=0;',
        'timeAmount += 1.0 / (Lx*Ly + Omega);':'events++;\n      timeAmount += 1.0 / (Lx*Ly + Omega);',
        'Substract[RandomLine * maxColumns + RandomColumn] += 1;':'Substract[RandomLine * maxColumns + RandomColumn] += 1;\n        depositions++;',
        'Ly++; // <- Column Quantity Update.':'Ly++; // <- Column Quantity Update.\n        duplications++;',
        'switch (RandomNeighbor) {':'diffusion++;\n          switch (RandomNeighbor) {',
        'timeParameter++; // <- Time Parameter Update.':
            'trace_measure(sampleCount,timeParameter,timeAmount,Lx,Ly,maxColumns,Substract,TopNeighbor,BottomNeighbor,LeftNeighbor,RightNeighbor,events,depositions,duplications,diffusion);\n        timeParameter++; // <- Time Parameter Update.',
    }
    for old,new in replacements.items():s=replace_once(s,old,new)
    return s


def args(config,seed=None):
    a=[]
    for name,value in config.items():a += ['--'+name,str(value)]
    if seed is not None:a += ['--seed',str(seed)]
    return a+['--output-dir','.']


def load(path):
    return [[float(x) for x in line.split()] for line in path.read_text().splitlines()]


def main():
    parser=argparse.ArgumentParser();parser.add_argument('--sanitize',action='store_true');a=parser.parse_args()
    assert hashlib.sha256(LEGACY.read_bytes()).hexdigest()==HASH,'Preserved legacy file changed'
    report=['Legacy SHA-256: '+HASH,'Compiler: '+run(CC+['--version'],ROOT).stdout.splitlines()[0]]
    totals=dict(cases=0,measurements=0,events=0,depositions=0,duplications=0,diffusion=0)
    ensemble={d:[] for d in (1,2,4)}
    flags=FLAGS+(['-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-pie','-no-pie'] if a.sanitize else ['-O2'])
    with tempfile.TemporaryDirectory(prefix='crsos-equivalence-') as temp:
        temp=Path(temp);binary=temp/'modular';trace=temp/'modular-trace'
        sources=[ROOT/f'src/{n}.c' for n in SOURCE_NAMES]
        run(CC+flags+sources+['-lm','-o',binary],temp)
        run(CC+flags+['-DCRS_TEST_TRACE','-include',ROOT/'tests/trace.h']+sources+['-lm','-o',trace],temp)
        cases=[]
        for d in (1,2,4):
            for seed in (12345679,34567891,87654321):
                cases.append((dict(delta=d,lx=16,**{'initial-ly':4,'max-columns':128,'max-time':12,'samples':3,'omega':1}),seed))
            cases.append((dict(delta=d,lx=13,**{'initial-ly':5,'max-columns':96,'max-time':10,'samples':2,'omega':0}),23456789))
            cases.append((dict(delta=d,lx=16,**{'initial-ly':3,'max-columns':160,'max-time':25,'samples':5,'omega':2.5}),76543219))
        for i,(config,seed) in enumerate(cases):
            case=temp/f'case{i}';case.mkdir();old=case/'old';new=case/'new';prod=case/'production'
            for p in (old,new,prod):p.mkdir()
            src=case/'oracle.c';src.write_text(oracle(config,seed));ob=case/'oracle'
            # The legacy oracle is compiled at the same optimization level. Its
            # constructor is seeded by the explicit textual adapter above.
            run(CC+FLAGS+(['-O1'] if a.sanitize else ['-O2'])+['-include',ROOT/'tests/trace.h',src,'-lm','-o',ob],case)
            run([ob],old);run([trace]+args(config,seed),new);run([binary]+args(config,seed),prod)
            d=config['delta']
            for before,after in [('LySize','LySize'),('MethodA','MethodA'),('MethodB','Bkappa'),('MethodC','Bm')]:
                expected=(old/f'{before}-C-RSOS-Simulation_01_Sample.dat').read_bytes()
                for directory in (new,prod):
                    actual=(directory/f'delta{d}-{after}.dat').read_bytes()
                    assert actual==expected,(i,after,'non-identical output')
                vals=load(prod/f'delta{d}-{after}.dat');assert len(vals)==config['max-time']
                assert all(math.isfinite(x) for row in vals for x in row)
            assert (old/'equivalence.trace').read_bytes()==(new/'equivalence.trace').read_bytes(),(i,'trajectory mismatch')
            lines=(new/'equivalence.trace').read_text().splitlines();totals['measurements']+=len(lines)
            events=depositions=dups=diffusion=0
            for sample in range(1,config['samples']+1):
                records=[line.split() for line in lines if line.split()[0]==str(sample)];last=records[-1]
                ev,dep,dup,diff=map(int,last[4:8]);assert ev==dep+dup
                assert int(last[3])==config['initial-ly']+dup
                events+=ev;depositions+=dep;dups+=dup;diffusion+=diff
            for k,v in [('cases',1),('events',events),('depositions',depositions),('duplications',dups),('diffusion',diffusion)]:totals[k]+=v
            if config['omega']==1:
                ensemble[d].append(load(prod/f'delta{d}-Bm.dat')[-1])
            report.append(f'delta={d} seed={seed} {config}: all 4 files byte-identical; traces identical; events={events}, depositions={depositions}, duplications={dups}, diffusion={diffusion}')
            # Refuse overwrite; a second invocation must leave existing files intact.
            before={p.name:p.read_bytes() for p in prod.iterdir()}
            r=run([binary]+args(config,seed),prod,ok=False);assert r.returncode!=0
            assert before=={p.name:p.read_bytes() for p in prod.iterdir()}
        for d,rows in ensemble.items():
            mean=[statistics.mean(c) for c in zip(*rows)];sd=[statistics.stdev(c) for c in zip(*rows)]
            report.append(f'delta={d} across 3 independent fixed seeds: final [time,height,S_B,K_Bm,W2] mean={mean}, SD={sd}; legacy-minus-modular=0 for every entry and aggregate.')
        for invalid in [['--delta','0'],['--seed','2'],['--max-time','0'],['--omega','nan'],['--lx','1'],['--samples','999999999'],['--delta','bad'],['--unknown','1'],['--seed']]:
            assert run([binary]+invalid,temp,ok=False).returncode==2,invalid
        guard=temp/'capacity';guard.mkdir()
        bad=dict(delta=1,lx=8,**{'initial-ly':4,'max-columns':4,'max-time':2,'samples':1,'omega':100})
        r=run([binary]+args(bad,12345679),guard,ok=False)
        assert r.returncode!=0 and 'capacity exhausted' in r.stderr
        automatic=temp/'automatic';automatic.mkdir()
        small=dict(delta=1,lx=8,**{'initial-ly':4,'max-columns':32,'max-time':2,'samples':1,'omega':1})
        run([binary]+args(small),automatic)
        chosen=int((automatic/'Choiced-Ran3-OddSeed.dat').read_text().strip())
        assert chosen%2==1 and 9999999<=chosen<=99999999
        # Rerun the automatically selected seed explicitly: same trajectory data.
        repeated=temp/'repeated';repeated.mkdir();run([binary]+args(small,chosen),repeated)
        for p in automatic.glob('delta*.dat'):assert p.read_bytes()==(repeated/p.name).read_bytes()
    assert hashlib.sha256(LEGACY.read_bytes()).hexdigest()==HASH
    assert totals['duplications']>0 and totals['diffusion']>0
    report += ['TOTALS: '+str(totals),'CLI, overwrite refusal, capacity guard, automatic seed/replay: PASS',
               'ASan/UBSan modular checks: '+('PASS' if a.sanitize else 'not requested'),
               'Scope: reduced runs only; equivalence on this compiler/architecture, not proof for every platform.']
    destination=ROOT/'build';destination.mkdir(exist_ok=True)
    (destination/('sanitizer-results.txt' if a.sanitize else 'equivalence-results.txt')).write_text('\n'.join(report)+'\n')
    print('\n'.join(report))


if __name__=='__main__':main()
