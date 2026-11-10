# Task: Configurable Location Precision Fuzzing and Meshtastic MediumFast Support

## Task Goal
1. Support configurable location precision (fuzzing) for Meshtastic:
   - Add `position_precision` (uint8_t, default 18 = ~90m) to `app_config_t` in `main/include/config_mgr.h`.
   - Update `main/config_mgr.c` to parse `position_precision` under `[meshtastic]` and output commented-out default in `config.ini`.
   - In `main/lora_time_beacons.c`, when `precision_bits > 0 && precision_bits < 32`:
     - Truncate `latitude_i` and `longitude_i` with `(uint32_t)(0xFFFFFFFFUL << (32 - precision_bits))`.
     - Append `precision_bits` field 7 (tag `0x38`, val `precision_bits`) to `Position` protobuf payload.
     - Allow `32` for exact GPS precision.
2. Fix Meshtastic MediumFast transmission:
   - In US915, switch RF carrier frequency to 913.125 MHz (`913125000` Hz, Slot 45) during MediumFast transmissions when default US915 frequency is configured.
   - Retune back to `cfg->lora_freq_hz` for LongFast and MeshCore.
   - Broadcast startup NodeInfo and Position on BOTH LongFast and MediumFast.
   - Rotate packets (NodeInfo, Position, Telemetry) symmetrically on both LongFast and MediumFast so MediumFast nodes receive NodeInfo and Position.
3. Update unit tests in `tests/test_harness.c` to verify:
   - Coordinate bitmasking and `precision_bits` field 7 encoding.
   - MediumFast frequency selection (913.125 MHz).
   - `position_precision` configuration parsing.
4. Verify build and native tests pass with zero errors and zero warnings.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `.gitignore`
- `main/include/config_mgr.h`
- `main/config_mgr.c`
- `main/lora_time_beacons.c`
- `tests/test_harness.c`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Add `position_precision` to `app_config_t` and `main/config_mgr.c`.
- [x] Implement coordinate fuzzing and `precision_bits` field in `main/lora_time_beacons.c`.
- [x] Implement MediumFast 913.125 MHz carrier frequency and packet rotation in `main/lora_time_beacons.c`.
- [x] Update unit tests in `tests/test_harness.c`.
- [x] Verify build and tests pass with zero errors and zero warnings.
