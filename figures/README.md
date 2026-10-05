# Selected figures

These are project results, not copies of figures from the reference papers. All four compare Δ=1,2,4, with the same black/circle, blue/square and orange/triangle identity.

| Public file | Local source (excluded from Git) | Adaptation |
|---|---|---|
| `width.svg` | `Analysis/figures/reconstruction_audit/recon05_W2.svg` | None; byte-identical copy |
| `beta-effective.svg` | `Analysis/figures/delta_crossover/delta01_beta_time.svg` | None; byte-identical copy |
| `beta-collapse.svg` | `Analysis/figures/delta_crossover/delta02_beta_beta_weighted.svg` | Only two panel titles translated into English; data and geometry unchanged |
| `crossover-scale.svg` | `Analysis/figures/delta_crossover/delta05_scale_law.svg` | None; byte-identical copy |

Figure captions and provenance:

- **Width:** mean spatial variance W²=⟨m₂⟩, log–log axes; pointwise approximate 95% bands from reconstructed blocks, narrow at this scale. This does not identify an asymptotic exponent.
- **Effective beta:** half the local log–log slope of W² in [t/√2,t√2]; 180 centers, complete windows only. Bands: pointwise 95% Student intervals using block jackknife. Horizontal line: the two-loop reference 0.19753, not an inferred asymptote.
- **Beta collapse:** τ=t/sΔ, with weighted relative scales s=(1,1.397651,2.049212). Right panel enlarges the late interval. Bands propagate jointly refitted scales by whole-block deletion; they are pointwise, not simultaneous.
- **Crossover scale:** reproduced **unweighted baseline** s=(1,1.401083,2.051630), one-SE bars, compared to √Δ and the covariance-aware GLS power fit φ=0.50927. This plot is intentionally the baseline, not the weighted 0.50739 direct-fit result. Δ=1 is an exact normalization, not an independent noisy datum.

The internal analyses used 20,20,12 blocks for Δ=1,2,4. The fourth plot and README give conditional precision, not systematic uncertainty from changing estimators/windows. Raw data and analysis scripts are deliberately excluded from this public snapshot; these figures cannot be regenerated from the public snapshot alone.

## Checksums of public SVG files

- `width.svg`: `852c524f9a40dab7e55784b68525f0e600fbf53c85d61d97cc5a26a1dc576f17`
- `beta-effective.svg`: `2a03d89904e403d9c4f8123303b0dd3fc47f20ec573b2d51a124b8e8c2c7f2e5`
- `beta-collapse.svg`: `f7005da643d933a70a09eb34a51c931d75e41a198ac057e833be5afd4d16283d`
- `crossover-scale.svg`: `76d3f15647612f75180c3be3f5f34c661f9eace14317f0a83c79f92ffe325b69`
