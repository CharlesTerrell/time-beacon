# Task: Fix GitHub Actions CI badge URL in README.md

## Task Goal
Fix the GitHub Actions CI badge image and link URL in `README.md` to match the actual repository name and owner (`CharlesTerrell/time-beacon` instead of `tachyon/time_beacon`).

## Allowed Files to Edit
- CURRENT_TASK.md
- README.md

## Verification Command
```bash
./build.sh test && python3 -c "
content = open('README.md').read()
assert 'tachyon/time_beacon' not in content, 'Outdated repo URL in README.md'
assert 'CharlesTerrell/time-beacon' in content, 'Correct repo URL missing in README.md'
print('Badge link check clean!')
"
```

## Checklist
- [x] Update CI badge and workflow URL in README.md to `CharlesTerrell/time-beacon`.
- [x] Run verification command with zero errors and zero warnings.
- [x] Mark checklist items complete and commit changes per AGENTS.md rules.
