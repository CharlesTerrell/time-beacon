# Task: Bring build_tracker.sh and environment scripts up to date

## Task Goal
Bring `build_tracker.sh` and `tools/idf_env.sh` up to date with the project's build, test, and environment loading workflows:
1. Update `tools/idf_env.sh` so fast-path checks verify `ESP_IDF_VERSION` and `IDF_PYTHON_ENV_PATH` before bypassing environment loading.
2. Update `build_tracker.sh` to source `tools/idf_env.sh` reliably to avoid IDF v6 environment crashes.
3. Add `test` subcommand to `build_tracker.sh` that executes the native host test suite (`tests/run_tests.sh`).
4. Update `build_tracker.sh` help/usage string to include `test`.
5. Update `README.md` to document `./build_tracker.sh test`.
6. Verify via `./build_tracker.sh test && ./build_tracker.sh all`.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `build_tracker.sh`
- `tools/idf_env.sh`
- `README.md`

## Verification Command
```bash
./build_tracker.sh test && ./build_tracker.sh all
```

## Checklist
- [x] Fix environment activation in `tools/idf_env.sh` and `build_tracker.sh`.
- [x] Add `test` command to `build_tracker.sh` invoking `tests/run_tests.sh`.
- [x] Update `README.md` with `./build_tracker.sh test` usage.
- [x] Run verification command with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
