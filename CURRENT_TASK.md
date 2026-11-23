# Task: Fix CI path filtering and ESP32 compiler errors (string.h, timegm)

## Task Goal
1. Prevent full CI rebuilds on documentation-only changes by adding `paths-ignore` to `.github/workflows/ci.yml`.
2. Fix ESP32 compilation failures:
   - Add missing `<string.h>` in `main/gnss_driver.c` for `strlen`.
   - Replace non-standard `timegm()` in `main/time_sync.c` with a portable, self-contained UTC-to-epoch converter.
   - Add unit test coverage for the UTC epoch conversion in `tests/test_harness.c`.

## Allowed Files to Edit
- CURRENT_TASK.md
- .github/workflows/ci.yml
- main/gnss_driver.c
- main/time_sync.c
- tests/test_harness.c

## Verification Command
```bash
./build.sh test && rm -rf build_v1 build_v2 && ./build.sh all
```

## Checklist
- [x] Add `paths-ignore` in `.github/workflows/ci.yml` for doc/markdown changes.
- [x] Include `<string.h>` in `main/gnss_driver.c`.
- [x] Replace `timegm()` with portable `utc_tm_to_epoch()` in `main/time_sync.c`.
- [x] Add unit tests for UTC epoch converter in `tests/test_harness.c`.
- [x] Run verification command with zero errors and zero warnings.
- [x] Mark checklist items complete and commit changes per AGENTS.md rules.
