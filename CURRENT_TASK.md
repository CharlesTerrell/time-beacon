# Task: Disable MeshCore LoRa Beacons by Default

## Task Goal
Disable MeshCore time beacons by default so that the LoRa radio focuses purely on Meshtastic (LongFast and MediumFast rotations alternating every minute), avoiding automated broadcast traffic on unsupported MeshCore network structures:
1. In `main/config_mgr.c`:
   - Set default `cfg->enable_meshcore = false`.
   - Update default config generator to write `# meshcore = false`.
2. In `main/lora_time_beacons.c`:
   - Ensure clean 2-phase rotation (LongFast / MediumFast) when `enable_meshcore == false`.
3. In `tests/test_harness.c`:
   - Update / add test assertions verifying `enable_meshcore` defaults to `false` and that 2-phase Meshtastic-only rotation alternates properly between LongFast and MediumFast.
4. Verification command:
   ```bash
   ./build_tracker.sh all && ./tests/run_tests.sh
   ```
5. Verify build and tests pass with zero errors and zero warnings, mark task complete, and auto-commit.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `main/config_mgr.c`
- `main/lora_time_beacons.c`
- `tests/test_harness.c`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Set `enable_meshcore = false` in `main/config_mgr.c`.
- [x] Verify 2-phase Meshtastic rotation schedule in `main/lora_time_beacons.c`.
- [x] Update tests in `tests/test_harness.c`.
- [x] Verify build and tests pass with zero errors and zero warnings.
- [x] Automatically commit verified changes per AGENTS.md.
