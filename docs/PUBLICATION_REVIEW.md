# Local publication review

This is a local release snapshot, not an online publication. Its first commit is local-only; no remote or push is part of this preparation. The Git root is the project directory itself, not its parent.

The selected license is the **BSD 3-Clause License**, SPDX identifier `BSD-3-Clause`. The complete text is in the root [LICENSE](../LICENSE), with copyright © 2026 Amir Bachar Saadeddine. The licensing decision is resolved; no license headers were added to the scientific sources.

## Scientific preservation

- All historical data, C sources, analysis results, PDFs and original figures remain on disk unchanged: **895 preexisting research files** were checked by SHA-256. The preparation records before/after hashes locally under the ignored `Analysis/results/publication_prep/` directory.
- The canonical source in `legacy/` is an unchanged byte copy. Its provenance, complete three-source comparison and hashes are in [EQUIVALENCE.md](EQUIVALENCE.md).
- The modular implementation preserves successful scientific trajectories, including numerical/output conventions that a future scientific revision may choose to improve explicitly. Operational guards and new filenames are documented in [OUTPUTS.md](OUTPUTS.md).
- Only reduced equivalence cases were run; no production simulation was executed. GCC, Clang and GCC ASan/UBSan checks passed. The sanitizer run required execution outside the process-inspection restrictions of the sandbox; no errors were suppressed.

## Publication boundary

The exact public file list is [PUBLIC_FILES.txt](PUBLIC_FILES.txt): **31 files**. It includes LICENSE, the README, Makefile, ignore rules, generic legacy source, modular source, tests, documentation and four SVG figures.

`.gitignore` excludes `.dat`, `.csv`, `.json`, `History/`, `Analysis/`, `References/`, the original `Samples-CRSOS*/` directories, `Relatorio.pdf`, the local graphical-analysis document, tool/credential directories, generated run output, binaries and caches. These items are not deleted.

The initial sandbox presented an empty, read-only `.git` directory. A check outside the sandbox also confirmed that no valid repository existed. An empty local repository on branch `main` was therefore initialized; there was no existing index or history to remove or rewrite. Following the author's approval, only the validated public manifest is staged for the first local commit. Historical data and research material remain local and ignored.

## Public-file and privacy audit

`make audit-public` checks the union of tracked and unignored untracked files against the manifest, tests ignore rules, validates Markdown links and scans the actual candidate content for known credential patterns, private home-directory paths, email addresses and network addresses. It rejects symlinks, files over 1 MiB, binary payloads and active/external SVG payloads. The SVG renderer metadata contains standard Matplotlib/W3C URLs, not private hosts.

No tokens, keys, credentials, private home paths, unnecessary emails, network addresses, caches or compiled binaries were found in the candidate. No scientific source was redacted. Researcher/supervisor names and the institution are intentionally retained as requested scientific attribution; they were verified on the local report cover. The audit is a reproducible pattern/content review, not a guarantee against every possible kind of secret. No earlier Git history existed to inspect.

Run from the root:

```bash
make
make test
make audit-public
git status --short
git ls-files --cached --others --exclude-standard
```

`make audit-public` does not stage files or modify Git metadata. To deliberately refresh the candidate manifest after changing the public layout, run `python3 tests/audit_public.py --write-manifest` and review its diff. Unknown files outside the intended directories make the audit fail.

## Review decisions still open

1. **Data availability:** raw data and the complete analysis remain local by request. Consequently, readers can reproduce the implementation and small equivalence tests, but cannot regenerate the historical scientific figures from this public snapshot alone. A future data release can address this without exposing the current workspace automatically.
2. **Scientific claims:** the README intentionally leaves β∞ and αroughness undetermined, distinguishes conditional precision from methodological sensitivity, and treats √Δ as an approximate hypothesis, not an established exact law.
3. **Historical output limitations:** within-block integer Ly averaging and moment conventions are preserved for equivalence. A future corrected-output format should be an explicit scientific/software revision, not an undocumented change to this one.

The selected figures are described in [figures/README.md](../figures/README.md). Only two panel titles in the collapse SVG were translated to English; the underlying curves, errors and axes are unchanged. No reference-paper images, report signatures or scanned report pages are included.
