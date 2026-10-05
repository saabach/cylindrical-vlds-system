# Scientific code provenance and equivalence

## Complete comparison before refactoring

All three archived files have **501 logical lines**. `wc -l` reports 500 for the first because it lacks a final newline.

| Archived parameter case | Line 73 | Line 74 | Final newline | Bytes |
|---|---|---|---|---:|
| Δ=1 | `Omega = 1, M = 1;` | `Lx = 32768, initLy = 4, maxColumns = 2800;` | Absent | 20739 |
| Δ=2 | `Omega = 1, M = 2;` | `Lx = 32768, initLy = 4, maxColumns = 4800;` | Present | 20740 |
| Δ=4 | `Omega = 1, M = 4;` | `Lx = 32768, initLy = 4, maxColumns = 4800;` | Present | 20740 |

Lines **1–72 and 75–501 have identical text**, including all RNG operations, deposition/diffusion rules, expansion, moments, normalization and output filenames. Line 501 differs only in its terminating newline in the Δ=1 comparison. Between Δ=2 and Δ=4, only line 73 differs. No algorithmic difference was found.

`maxColumns` is storage capacity/stride, not the number of active columns. It changes allocation and addresses but not the physical neighbor graph or event selection while capacity suffices. The original has no guard against overflow of that capacity. This distinction is why selecting the Δ=1 implementation as canonical is justified; the other historical settings remain reproducible by parameters.

## Canonical copy and hashes

Source: `Samples-CRSOS1/S1_Cyl_01_Sample.c` (retained locally, ignored by Git).

Copy: `legacy/CRSOS_Cylindrical_Concept.c` (public, **no content edits**).

Both SHA-256 values are exactly:

```text
5e60fe09e9ba96bb283747dfb464b1e9d792a6c438b9c75388f2263072fe5b1a
```

The other originals, also unchanged locally:

```text
Δ=2  bea30850538758c9cebd9e617ea6d4fb743fbc1ac36934fe8f5b87b2e57fbaae
Δ=4  08d92cd1705790d3f7303a15ae162ee0dc7f5362743af80b9458059f81e9acb6
```

Check the public copy with `sha256sum legacy/CRSOS_Cylindrical_Concept.c`. The test suite also checks its hash before and after running. The complete local diffs are retained under the ignored `Analysis/results/publication_prep/` directory. The description above lists every changed logical line.

## Structural separation

| Module | Responsibility |
|---|---|
| `main.c`, `config.h` | CLI, historical defaults, sample/event/measurement loops, time |
| `lattice.c/.h` | Allocation, initial periodic neighbors, reset, column duplication/splicing |
| `crsos.c/.h` | Restricted deposition, conservative random walk, diagnostic event counters |
| `rng.c/.h` | Original Ran3/stream algorithms and argument order, stream seeding/warm-up |
| `observables.c/.h` | Original per-site moment arithmetic and precision conversions |
| `sampling.c/.h` | Accumulation and normalization across realizations |
| `output.c/.h` | Parameter-based names, original output formulas, run provenance |

No module name depends on a particular Δ. Defaults are the canonical Δ=1 defaults, including capacity 2800. To reproduce the other archived configurations, set Δ=2/4 and capacity 4800. RNG state remains internal, single-process state; threading was not introduced.

## Oracle adapter: exactly what changes for a reduced test

Running the untouched executable would invoke the full production configuration and automatic seed selection. Therefore the test creates a temporary copy of the preserved source and applies **asserted, single-occurrence substitutions**:

1. Replace the constructor's `RAN3_SEED = GetOddNum()` with a fixed test seed.
2. Replace the three original configuration assignment lines with reduced parameters.
3. Add passive counters for events, accepted deposition, duplication and diffusion.
4. Add a measurement-time trace of time (hexadecimal double), Ly, event counters and a 64-bit fingerprint of every active height and neighbor array.

The adapter is readable in `tests/test_equivalence.py`; it does not change the scientific updates or measurement expressions. All temporary executables, copies and outputs are removed with the temporary directory. The legacy public file is never patched, reformatted or compiled with a hidden physical correction.

## Test matrix and observed results

For **each** Δ=1,2,4:

| Cases | Lx | Initial Ly | Capacity | Tmax | Samples | Ω | Seeds |
|---|---:|---:|---:|---:|---:|---:|---|
| Three expanding runs | 16 | 4 | 128 | 12 | 3 | 1 | 12345679, 34567891, 87654321 |
| No-expansion control | 13 | 5 | 96 | 10 | 2 | 0 | 23456789 |
| Faster-expansion control | 16 | 3 | 160 | 25 | 5 | 2.5 | 76543219 |

**15 configurations**, 759 measurement-time trajectory records, 264426 deposition/expansion events: 263103 depositions and 1323 duplications, plus 278978 diffusion hops. These counts describe one traversal of the test matrix; the oracle, traced refactor and normal refactor each run it.

Results with GCC 16.2.1 and Clang 22.1.8 at `-O2`, on this Linux/x86-64 environment:

- All four numerical output files were **byte-for-byte identical** after mapping the documented filenames: no tolerance was needed.
- Time, integer Ly, mean height, W² and both skewness/kurtosis averaging conventions all matched at every stored row.
- Measurement-time lattice/neighbor fingerprints and all event/diffusion counters matched exactly.
- The normal modular executable and test-instrumented executable produced identical numerical files.
- Across the three distinct fixed seeds per Δ, both the ensemble means and sample standard deviations matched exactly (paired differences zero). This statistical comparison is stronger than accepting discrepancies as noise; it is not a validation of a physical universality class.
- CLI rejection, capacity exhaustion, refusal to overwrite, missing seed-log creation, and replay of an automatically selected seed passed.

`make test` regenerates a detailed local log in `build/equivalence-results.txt`, including compiler version and ensemble statistics. `make test-sanitize` additionally checks the modular executable and failure paths with AddressSanitizer/UndefinedBehaviorSanitizer at `-O1`; its separate log is `build/sanitizer-results.txt`. On restricted environments LeakSanitizer may require permission to inspect processes; this is a tooling constraint, not a scientific divergence.

The tests demonstrate equivalence for these seeded finite runs and tested toolchains. They do not prove all-parameter equivalence, RNG independence, bitwise portability to other floating-point architectures, or asymptotic physics. No production simulation was run.

GCC ASan/UBSan, including leak checks, also completed the same matrix successfully. A temporary export containing only the 30 public manifest files built and passed the equivalence suite without `Analysis/`, historical data or other local research material.

## Deliberate operational differences

The modular version adds CLI parameters, parameter-based filenames, a run record, explicit seed replay, input/allocation/I/O checks, capacity/overflow guards and overwrite refusal. It creates a missing seed log instead of dereferencing the original's unchecked failed `fopen`. Wall-clock progress messages differ. These are documented software changes outside successful scientific updates; the numerical `.dat` contents remain identical in the tested domain.

Integer Ly averaging and double/long-double conversions are **not corrected** as part of this refactor. See [OUTPUTS.md](OUTPUTS.md). A future improvement should version those semantics and add new tests instead of silently changing the historical estimator.
