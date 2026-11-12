# Task: Fix Indentation and Missing Function Signature in tools/idf_env.sh

## Task Goal
Fix truncated `_find_idf_export()` declaration and mismatched indentation in `tools/idf_env.sh`:
- Export active paths directly to avoid Python subprocesses hanging when sourced.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `tools/idf_env.sh`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Clean up syntax and structure in `tools/idf_env.sh`.
- [x] Verify build and tests pass with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
