# Task: Configure Hybrid Nightly Builds with Commit Check in GitHub Actions

## Task Goal
Configure `.github/workflows/ci.yml` so that:
1. Fast host unit tests run on `push` and `pull_request` (as well as scheduled/manual triggers).
2. The heavy firmware builds, release uploads, and GitHub Pages deployments run only on:
   - Scheduled nightly runs (`schedule: - cron: '0 6 * * *'`),
   - Manual UI triggers (`workflow_dispatch`), or
   - Version tags (`refs/tags/v*`).
3. For scheduled nightly runs, check whether any commits have been made within the last 24 hours. If no commits occurred, skip building and publishing.
4. Name the rolling pre-release tag `nightly` (title "Time Beacon (Nightly Build)"), while keeping official version tags (`v*`) intact.
5. Update `README.md` release links if necessary.

## Allowed Files to Edit
- CURRENT_TASK.md
- .github/workflows/ci.yml
- README.md

## Verification Command
```bash
./tests/run_tests.sh && python3 -c "import yaml; yaml.safe_load(open('.github/workflows/ci.yml'))"
```

## Checklist
- [x] Update `CURRENT_TASK.md` with goals and allowed files.
- [x] Update `.github/workflows/ci.yml` with `schedule` and `workflow_dispatch` triggers, commit freshness check, and `nightly` tag name.
- [x] Update README references to releases if necessary.
- [x] Run verification command with zero errors and zero warnings.
- [x] Commit changes per AGENTS.md rules.
