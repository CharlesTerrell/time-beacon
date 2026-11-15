# Task: Filter Temporary Deactivation Path to Prevent Dotenv Null Byte Errors

## Task Goal
1. In `tools/idf_env.sh`:
   - Filter out `IDF_DEACTIVATE_FILE_PATH` when exporting to `.env.esp` to avoid temporary file race conditions and `\0` null-byte dotenv syntax errors in mise.
2. Verification command:
   ```bash
   ./build_tracker.sh all && ./tests/run_tests.sh
   ```
3. Auto-commit on clean build and tests per `AGENTS.md`.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `tools/idf_env.sh`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Filter out `IDF_DEACTIVATE_FILE_PATH` in `tools/idf_env.sh`.
- [x] Run verification command with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
