# Task: Update Agent Rules, Task Management, and Recommended CLI Tools

## Task Goal
Update `AGENTS.md` and `CURRENT_TASK.md` to reflect current project architecture:
1. Support ESP-IDF v5.x and v6.x targets (`-std=gnu17` / `-std=gnu23`).
2. Remove obsolete `TODO.md` references (archived to `docs/ref/early-roadmap.md`).
3. Add recommended development and debugging tools (`tio`, `rg`, `idf.py monitor`).
4. Maintain strict contract: static memory, error handling, fixed-width types, board pin constants, and auto-commit on clean verification.

## Allowed Files to Edit
- `AGENTS.md`
- `CURRENT_TASK.md`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Update `AGENTS.md` with ESP-IDF v5.x/v6.x target, remove `TODO.md`, and document CLI tools.
- [x] Verify build and tests pass with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
