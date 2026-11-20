# Task: Rename build_tracker.sh to build.sh

## Task Goal
Rename `build_tracker.sh` to generic `build.sh` and update documentation, CI, and agent instructions:
1. Rename `build_tracker.sh` to `build.sh` and create backward-compatibility symlink `build_tracker.sh -> build.sh`.
2. Update `.github/workflows/ci.yml` to call `./build.sh all`.
3. Update `README.md` references from `./build_tracker.sh` to `./build.sh`.
4. Update `AGENTS.md` build tool recommendation.
5. Verify tests run cleanly via `./build.sh test` and `./build_tracker.sh test`.

## Allowed Files to Edit
- CURRENT_TASK.md
- build.sh
- build_tracker.sh
- README.md
- AGENTS.md
- .github/workflows/ci.yml

## Verification Command
```bash
./build.sh test && ./build_tracker.sh test && python3 -c "import yaml; yaml.safe_load(open('.github/workflows/ci.yml'))"
```

## Checklist
- [x] Rename build_tracker.sh to build.sh and create symlink.
- [x] Update README.md, AGENTS.md, and .github/workflows/ci.yml.
- [x] Run verification command with zero errors and zero warnings.
- [x] Mark checklist items complete and commit changes per AGENTS.md rules.
