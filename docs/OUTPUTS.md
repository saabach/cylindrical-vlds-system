# Output definitions and compatibility

The default run directory is `output/`. Files are named by the parameter, not by archived series labels. Existing files are refused. Parent directories must exist; the final run directory can be created by the program. An unsuccessful run may leave partial files: use only outputs from a successful exit with `status=complete` in the run record.

| File | Columns / contents |
|---|---|
| `deltaN-LySize.dat` | Mean actual measurement time; **integer-truncated** mean Ly |
| `deltaN-MethodA.dat` | Time; mean height; mean per-realization skewness; mean per-realization excess kurtosis; mean m₂ |
| `deltaN-Bkappa.dat` | Time; mean height; S_B; K_Bκ; mean m₂ |
| `deltaN-Bm.dat` | Time; mean height; S_B; K_Bm; mean m₂ |
| `deltaN-run.txt` | CLI parameters, seed, sample index, final time, Ly, event/deposition/duplication/diffusion counts, completion status |
| `Choiced-Ran3-OddSeed.dat` | Automatically chosen seeds for this output directory; not needed for explicit `--seed` |

Numerical `.dat` files retain the historical headerless, tab-separated format: floating values use `%.34g`, Ly uses `%d`. There are `--max-time` rows. Each is an accumulated measurement threshold, **not an independent realization**. Time is the average of the actual times immediately after the first event that crosses each integer threshold.

## Moments and the order of averaging

For one realization with N=Lx Ly sites,

$$\bar h=\frac1N\sum_i h_i,\qquad m_n=\frac1N\sum_i(h_i-\bar h)^n.$$

Angle brackets below average over `--samples` realizations in **one process/block**. The mean width is $W^2=\langle m_2\rangle$.

$$S_A=\left\langle\frac{m_3}{m_2^{3/2}}\right\rangle,\qquad
K_A=\left\langle\frac{m_4-3m_2^2}{m_2^2}\right\rangle,$$

$$S_B=\frac{\langle m_3\rangle}{\langle m_2\rangle^{3/2}},\qquad
K_{B_\kappa}=\frac{\langle m_4-3m_2^2\rangle}{\langle m_2\rangle^2},$$

$$K_{B_m}=\frac{\langle m_4\rangle-3\langle m_2\rangle^2}{\langle m_2\rangle^2}.$$

Thus $K_{B_m}-K_{B_\kappa}=3[\langle m_2^2\rangle-\langle m_2\rangle^2]/\langle m_2\rangle^2$.

Historical `MethodA` maps to `MethodA`; `MethodB` maps to `Bkappa`; historical `MethodC` maps to **`Bm`**. The last is not method C in Oliveira, PRE 105, 064803 (2022). The reference's globally pooled-height method needs additional raw/mean-height mixed-moment information not preserved by these block summaries. It is not synthesized here.

Combining blocks requires recovering/combining the moments before forming B ratios; simply averaging their S/K columns generally computes another estimator. Separate process directories and seed records preserve block provenance. The implementation still exports historical observables, not every realization's complete spatial field or uncertainty.

## Details deliberately retained

- Height and central-moment sums accumulate in `long double`, but helper functions `Square`, `Cubic`, `Quadric` accept/return `double`; accumulation databases use `double`. Changing these types changes the numerical oracle.
- The mean-height and central-moment loops divide each site's contribution before accumulation, in the original row/column order.
- Ly is summed in an integer array then divided by the sample count using integer division. The modular version does not silently replace this with a floating mean. The separate historical reconstruction repaired **between-block** truncation, not the irreversible within-block truncation.
- The probability numerator is `(float)(Lx*Ly)` before division by a double denominator. This matters above the float exact-integer range and is not “corrected” by refactoring.
- Neighbors are periodic in both directions. Appended y storage slots do not retain geometric ordering; spatial analysis must follow the neighbor ring.
- A diffusing particle always deposits somewhere; diffusion hops neither create separate clock increments nor reseed the streams.
- Zero spatial variance makes normalized skewness/kurtosis undefined; the legacy formulas are retained rather than assigning an artificial zero.

## Operational guards added in the modular code

CLI validation, allocation/file error checks, fixed-capacity and height-overflow checks, seed-log creation when absent, and refusal to overwrite a run replace failure/undefined behavior on invalid inputs. They do not change a successful, in-capacity trajectory. Capacity is not automatically expanded, because the requested refactor preserves the existing memory-stride configuration. Provide sufficient `--max-columns`; an expansion beyond it exits with an error.

The RNG algorithms and their historical argument ordering are retained. Explicit seeding initializes the process once; each realization gets stream seeds from the same continuing Ran3 generator, with the original 999 warm-up multiplications. A seed plus all parameters and the same numerical environment enables replay. No guarantee of byte identity across arbitrary architectures, math libraries or fast-math compiler settings is claimed.
