# Task: Add size command to build.sh

## Task Goal
Add `size` target to `build.sh`:
1. Implement `size_target` in `build.sh` invoking `idf.py -B <bdir> size` (and optional `size-components`).
2. Update usage help string in `build.sh`.
3. Update `README.md` and `AGENTS.md` with `./build.sh size` usage.
4. Verify tests pass and `./build.sh size v1.1` runs successfully.

## Allowed Files to Edit
- CURRENT_TASK.md
- build.sh
- README.md
- AGENTS.md

## Verification Command
```bash
./build.sh test && ./build.sh size v1.1
```

## Checklist
- [x] Add size target to build.sh.
- [x] Update README.md and AGENTS.md.
- [x] Run verification command with zero errors and zero warnings.
- [x] Mark checklist items complete and commit changes per AGENTS.md rules.
