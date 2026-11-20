# Task: Set up GitHub Actions CI workflow

## Task Goal
Create GitHub Actions CI workflow in `.github/workflows/ci.yml` that:
1. Runs native host unit tests on PR and push to `main` or `master`.
2. Builds firmware targets for ESP32-S3 (`v1.1` and `v2`) inside the official ESP-IDF container.
3. Packages and archives compiled binaries (`heltec-tracker-ntp.bin`, `bootloader.bin`, `partition-table.bin`) as GitHub workflow artifacts.
4. Validates CI YAML syntax and verifies host test execution.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `.github/workflows/ci.yml`

## Verification Command
```bash
python3 -c "import yaml; yaml.safe_load(open('.github/workflows/ci.yml'))" && ./build_tracker.sh test
```

## Checklist
- [x] Create `.github/workflows/ci.yml` with host test and ESP32-S3 firmware build jobs.
- [x] Validate workflow syntax and run local verification command.
- [x] Mark checklist items complete and commit changes per AGENTS.md rules.
