# Task: Fix Meshtastic Position Protobuf Encoding

## Task Goal
Fix Meshtastic Position protobuf message encoding in `main/lora_time_beacons.c`:
1. Align Position field numbers and wire types to Meshtastic `mesh.proto`:
   - `precision_bits`: field 23 (tag `0xB8, 0x01`, varint) instead of invalid tag `0x38`.
   - `location_source`: field 5 (tag `0x28`, varint) with value `2` (`LOC_INTERNAL`) instead of invalid field 8.
   - `altitude_source`: field 6 (tag `0x30`, varint) with value `2` (`ALT_INTERNAL`) instead of invalid field 9.
   - `time`: field 4 (tag `0x25`, fixed32).
   - `timestamp`: field 7 (tag `0x3D`, fixed32) carrying epoch seconds.
2. Update `tests/test_harness.c`:
   - Update `test_meshtastic_position_layout()` to assert the correct proto tags (`0x0D`, `0x15`, `0x25`, `0x28`, `0x30`, `0x3D`, `0xB8 0x01`).
3. Verification command:
   ```bash
   ./build_tracker.sh all && ./tests/run_tests.sh
   ```
4. Verify build and tests pass with zero errors and zero warnings, mark task complete, and auto-commit.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `main/lora_time_beacons.c`
- `tests/test_harness.c`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Correct protobuf field tags and values in `main/lora_time_beacons.c`.
- [x] Update assertions in `tests/test_harness.c`.
- [x] Verify build and tests pass with zero errors and zero warnings.
- [x] Automatically commit verified changes per AGENTS.md.
