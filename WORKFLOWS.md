# Workflows

Repository-owned GitHub Actions for `jooservices/OpenKey` (GitHub-hosted
runners, `ubuntu-latest`).

| Workflow | Trigger | Purpose |
| --- | --- | --- |
| `ci.yml` | PR to `master`; push to `master` | Build + run engine unit tests, enforce 85% line-coverage floor on the engine C++ core, upload coverage report |
| `commitlint.yml` | PR opened/edited/sync/reopened | Validate Conventional Commits on PR commits (`.github/commitlint.config.mjs`) |
| `semantic-pr.yml` | PR opened/edited/sync | Validate the PR title follows Conventional Commits |
| `codeql.yml` | PR to `master`; push to `master`; weekly | CodeQL static analysis for C/C++ and GitHub Actions |
| `scorecard.yml` | push to `master`; weekly; manual | OpenSSF Scorecard, SARIF uploaded to security-events |

Run everything locally from `tests/`:

```bash
make test       # build + run all engine unit tests
make coverage   # per-file coverage report (clang --coverage + gcov)
./ci.sh         # full gate: tests + 85% coverage floor
```

Note: this repo is a 3rd-party fork (`tuyenvm/OpenKey`) adopted for reference.
The CI gate covers the engine C++ core only; the macOS/Windows UI layer has no
automated tests.