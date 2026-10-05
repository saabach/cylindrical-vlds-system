#!/usr/bin/env python3
"""Check the exact Git-visible publication set without staging or publishing it."""
import argparse
import hashlib
from pathlib import Path
import re
import subprocess
import sys
import xml.etree.ElementTree as ET

ROOT=Path(__file__).resolve().parents[1]
TOP={'LICENSE','Makefile','README.md'}
DIRECTORIES={'legacy','src','tests','figures','docs'}
MANIFEST=ROOT/'docs/PUBLIC_FILES.txt'


def git(*args,input=None):
    r=subprocess.run(['git',*args],cwd=ROOT,input=input,capture_output=True)
    if r.returncode:raise RuntimeError(r.stderr.decode(errors='replace').strip())
    return r.stdout


def expected_files():
    files=set(TOP)
    for directory in DIRECTORIES:
        for p in (ROOT/directory).rglob('*'):
            if p.is_file() and '__pycache__' not in p.parts:files.add(p.relative_to(ROOT).as_posix())
    files.add('docs/PUBLIC_FILES.txt')
    return sorted(files)


def main():
    parser=argparse.ArgumentParser();parser.add_argument('--write-manifest',action='store_true');args=parser.parse_args()
    expected=expected_files()
    if args.write_manifest:MANIFEST.write_text('\n'.join(expected)+'\n')
    assert MANIFEST.read_text().splitlines()==expected,'Public manifest is out of date'
    assert Path(git('rev-parse','--show-toplevel').decode().strip()).resolve()==ROOT,'Wrong Git root'
    visible=set(git('ls-files','--cached','--others','--exclude-standard','-z').decode().split('\0'))-{''}
    assert visible==set(expected),f'Unexpected Git files: {sorted(visible-set(expected))}; missing: {sorted(set(expected)-visible)}'
    probes=['History/probe.c','References/probe.pdf','Analysis/probe.py','Relatorio.pdf',
            'Samples-CRSOS1/probe.c','probe.dat','probe.csv','probe.json','build/probe.o',
            'output/delta1-run.txt','.aws/credentials','.codex/config.toml',
            '.agents/local.md','.env','cylindrical-crsos','tests/__pycache__/probe.pyc']
    ignored=git('check-ignore','--no-index','--stdin',input=('\n'.join(probes)+'\n').encode()).decode().splitlines()
    assert set(ignored)==set(probes),f'Missing ignores: {set(probes)-set(ignored)}'
    # Construct known patterns without embedding a real credential or path.
    patterns={
        'private key':r'-----BEGIN (?:RSA |EC |OPENSSH )?PRIVATE KEY-----',
        'AWS access identifier':r'\b(?:AKIA|ASIA)[A-Z0-9]{16}\b',
        'GitHub token':r'\bgh[pousr]_[A-Za-z0-9]{30,}\b|\bgithub_pat_[A-Za-z0-9_]{40,}\b',
        'Slack token':r'\bxox[baprs]-[A-Za-z0-9-]{20,}\b',
        'home path':r'(?:/'+r'home|/'+r'Users)/[A-Za-z0-9_.-]+/',
        'email':r'\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}\b',
        'literal credential assignment':r'(?i)(?:password|api_key|secret_token)\s*[:=]\s*[\x22\x27][A-Za-z0-9_+/=-]{16,}[\x22\x27]',
        'network address':r'\b(?:\d{1,3}\.){3}\d{1,3}\b',
    }
    findings=[];sizes=[]
    for rel in expected:
        p=ROOT/rel
        assert not p.is_symlink(),f'Public symlink: {rel}'
        raw=p.read_bytes();sizes.append((len(raw),rel))
        assert len(raw)<1024*1024,f'Large public file: {rel}'
        assert b'\0' not in raw,f'Binary payload: {rel}'
        text=raw.decode('utf-8')
        for name,pattern in patterns.items():
            for m in re.finditer(pattern,text):findings.append((rel,name,text[:m.start()].count('\n')+1))
        if p.suffix=='.svg':
            tree=ET.fromstring(text)
            for e in tree.iter():
                assert e.tag.split('}')[-1] not in ('script','foreignObject','image'),rel
                for key,value in e.attrib.items():
                    assert not key.lower().startswith('on'),(rel,key)
                    if key.split('}')[-1]=='href':assert value.startswith('#'),(rel,value)
        # Relative Markdown/image links must not depend on the ignored workspace.
        if p.suffix=='.md':
            targets=re.findall(r'\]\(([^)]+)\)',text)+re.findall(r'<img\s+src="([^"]+)"',text)
            for target in targets:
                if target.startswith(('https://','http://','#')):continue
                q=(p.parent/target.split('#')[0].strip('<>')).resolve()
                assert q.is_file() and q.relative_to(ROOT).as_posix() in expected,(rel,target)
    assert not findings,findings
    assert hashlib.sha256((ROOT/'legacy/CRSOS_Cylindrical_Concept.c').read_bytes()).hexdigest()=='5e60fe09e9ba96bb283747dfb464b1e9d792a6c438b9c75388f2263072fe5b1a'
    tracked=git('ls-files','--cached','-z').decode().split('\0');tracked=[p for p in tracked if p]
    report=[f'Public candidates: {len(expected)}',f'Currently tracked/indexed: {len(tracked)}',
            f'Total candidate bytes: {sum(x[0] for x in sizes)}',f'Largest file: {max(sizes)}',
            'All private-data/build/tool probes ignored: PASS',
            'Credential/home-path/email/address patterns: no findings',
            'SVG external payload checks and local documentation links: PASS',
            'Legacy checksum: PASS','No staging, commit, remote configuration or push performed by this audit.']
    print('\n'.join(report))
    build=ROOT/'build';build.mkdir(exist_ok=True)
    (build/'public-audit.txt').write_text('\n'.join(report)+'\n')


if __name__=='__main__':
    try:main()
    except (AssertionError,RuntimeError) as e:
        print(f'PUBLIC AUDIT FAILED: {e}',file=sys.stderr);sys.exit(1)
