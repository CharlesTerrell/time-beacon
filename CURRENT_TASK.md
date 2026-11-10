# Task: Fix Meshtastic HardwareModel Enum to HELTEC_WIRELESS_TRACKER

## Task Goal
1. In `main/lora_time_beacons.c`:
   - Change `hw_model` from 47 (`RPI_PICO`) to 48 (`HELTEC_WIRELESS_TRACKER`) in `build_meshtastic_nodeinfo_packet`.
2. In `tests/test_harness.c`:
   - Update `test_meshtastic_nodeinfo_layout()` to assert hardware model 48.
3. Run verification command:
   ```bash
   ./build_tracker.sh all && ./tests/run_tests.sh
   ```
4. Verify build and native tests pass with zero errors and zero warnings, mark task complete, and auto-commit.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `main/lora_time_beacons.c`
- `tests/test_harness.c`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Update `hw_model` to 48 in `main/lora_time_beacons.c`.
- [x] Update unit test in `tests/test_harness.c`.
- [x] Verify build and tests pass with zero errors and zero warnings.
- [x] Automatically commit verified changes per AGENTS.md.
