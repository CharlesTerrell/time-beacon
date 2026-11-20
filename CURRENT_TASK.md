# Task: Add inspection CLI tools to AGENTS.md

## Task Goal
Update `AGENTS.md` with recommended inspection and analysis tools:
1. Add `ctags`/`cscope` for fast symbol and definition lookups.
2. Add `clang-tidy` for static code analysis.
3. Add `idf.py size` / `idf.py size-components` for static memory and flash tracking.
4. Add `xtensa-esp32s3-elf-nm -S --size-sort` for ELF symbol size inspection.
5. Verify test suite passes cleanly.

## Allowed Files to Edit
- CURRENT_TASK.md
- AGENTS.md

## Verification Command
```bash
./build.sh test && test -f AGENTS.md
```

## Checklist
- [x] Update Recommended CLI & Inspection Tools in AGENTS.md.
- [x] Run verification command with zero errors and zero warnings.
- [x] Mark checklist items complete and commit changes per AGENTS.md rules.
