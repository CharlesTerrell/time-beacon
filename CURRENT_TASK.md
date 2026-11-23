# Task: Update Firmware Binary Naming and Multi-Version CI Matrix

## Task Goal
1. Update binary naming to follow the Meshtastic board convention with `.factory.bin` and version/build ID:
   - Base board: `time-beacon-heltec-wireless-tracker-<version>.<build_id>.factory.bin`
   - V2 board: `time-beacon-heltec-wireless-tracker-v2-<version>.<build_id>.factory.bin`
   - Also keep unmerged app binaries (`.bin`) available for future OTA.
   - For web flasher compatibility, maintain a clean canonical alias in `web/` or manifest pointing to the active factory binary.
2. Update `.github/workflows/ci.yml`:
   - Run firmware builds across a matrix: `espressif/idf:v6.1` (primary release builder) and `espressif/idf:v5.3.2` (compatibility check).
   - Only publish release assets and deploy web installer from the `v6.1` job.
3. Update `build.sh`, `web/manifest-*.json`, `web/index.html`, and `README.md` to reflect the new naming conventions.

## Allowed Files to Edit
- CURRENT_TASK.md
- build.sh
- .github/workflows/ci.yml
- web/manifest-tracker-v1.1.json
- web/manifest-tracker-v2.json
- README.md

## Verification Command
```bash
./tests/run_tests.sh && python3 -c "import yaml; yaml.safe_load(open('.github/workflows/ci.yml'))"
```

## Checklist
- [x] Update `CURRENT_TASK.md`.
- [x] Update `build.sh` to extract version/build ID and output `.factory.bin` and `.bin` with the new naming scheme.
- [x] Update `.github/workflows/ci.yml` with the dual v6.1 / v5.3.2 matrix and upload the new binary names.
- [x] Update `web/manifest-tracker-v1.1.json` and `web/manifest-tracker-v2.json`.
- [x] Update `README.md` binary documentation.
- [x] Run verification command with zero errors and zero warnings.
- [x] Commit changes per AGENTS.md rules.
