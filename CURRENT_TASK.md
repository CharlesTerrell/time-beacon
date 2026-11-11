# Task: Build Environment Portability and Top-Level README

## Task Goal
Improve developer onboarding and build portability:
1. Create `tools/idf_env.sh`: Fast, source-able environment activator that checks `IDF_PATH`, standard `~/.espressif/v*/esp-idf` and `~/esp/esp-idf` locations, with caching to avoid slow exports.
2. Update `build_tracker.sh` to leverage portable discovery without hardcoding user-specific paths.
3. Create top-level `README.md` with:
   - Project overview (Stratum-1 GPS Time Beacon with Wi-Fi NTP, BLE CTS, and Meshtastic LoRa).
   - Supported hardware.
   - Prerequisites & ESP-IDF setup.
   - Build, test, and flash instructions.
   - Configuration via `time_beacon.ini`.
   - Links to design documentation in `docs/plan/`.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `build_tracker.sh`
- `tools/idf_env.sh`
- `README.md`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Create `tools/idf_env.sh`.
- [x] Update `build_tracker.sh` for portable IDF detection.
- [x] Create `README.md`.
- [x] Verify build and tests pass with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
