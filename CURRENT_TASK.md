# Task: Implement 3-way 1-minute rotation (LongFast -> MediumFast -> MeshCore)

## Task Goal
Implement Option B rotation in `main/lora_time_beacons.c`:
1. When both Meshtastic and MeshCore are enabled:
   - Cycle 0 (min 0): Meshtastic LongFast burst (NodeInfo, Position, Telemetry with ~500ms spacing).
   - Cycle 1 (min 1): Meshtastic MediumFast burst (NodeInfo, Position, Telemetry with ~500ms spacing).
   - Cycle 2 (min 2): MeshCore pulse (ISO-8601 time + GPS coordinates text).
   - Next cycle repeats every 3 minutes, with 1 turn executing every minute.
2. If only Meshtastic is enabled: alternate LongFast and MediumFast bursts every minute.
3. If only MeshCore is enabled: send MeshCore pulse every minute.
4. On startup: broadcast immediate announcement burst across enabled modes so peers populate NodeDB quickly.
5. In `tests/test_harness.c`: add unit test verifying the 3-way rotation scheduling and packet builder layouts.
6. Verification command:
   ```bash
   ./build_tracker.sh all && ./tests/run_tests.sh
   ```

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `main/lora_time_beacons.c`
- `tests/test_harness.c`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Update `lora_beacon_task` in `main/lora_time_beacons.c` to 3-way 1-minute rotation with burst transmissions.
- [x] Update `tests/test_harness.c` to verify rotation logic.
- [x] Verify build and tests pass with zero errors and zero warnings.
- [x] Automatically commit verified changes per AGENTS.md.
