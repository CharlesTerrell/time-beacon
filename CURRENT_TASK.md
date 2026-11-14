# Task: Unpin Ninja and Add Automatic Environment Cache Hook to mise.toml

## Task Goal
1. In `mise.toml`:
   - Remove hardcoded `ninja = "1.12.1"` from `[tools]`. Rely on ESP-IDF's bundled ninja in `.env.esp` PATH.
   - Clean up `mise.toml` to use `tools/idf_env.sh --refresh` for `update-esp-env`.
2. Verification command:
   ```bash
   ./build_tracker.sh all && ./tests/run_tests.sh
   ```
3. Auto-commit on clean build and tests per `AGENTS.md`.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `mise.toml`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Remove hardcoded ninja from `mise.toml`.
- [x] Ensure clean `.env.esp` integration with `mise.toml`.
- [x] Run verification command with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
