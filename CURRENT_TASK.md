# Task: Fix Release Asset Duplication in CI Workflow

## Task Goal
Fix the duplicate release asset upload in `.github/workflows/ci.yml` where `*.factory.bin` was matched twice due to overlapping glob patterns. Use non-overlapping single glob patterns for each board's `.bin` files and enable asset replacement settings.

## Allowed Files to Edit
- CURRENT_TASK.md
- .github/workflows/ci.yml

## Verification Command
```bash
./tests/run_tests.sh && python3 -c "import yaml; yaml.safe_load(open('.github/workflows/ci.yml'))"
```

## Checklist
- [x] Update `CURRENT_TASK.md`.
- [x] Update `.github/workflows/ci.yml` `files` list under `softprops/action-gh-release@v2` to avoid duplicate glob matching.
- [x] Run verification command with zero errors and zero warnings.
- [x] Commit changes per AGENTS.md rules.
