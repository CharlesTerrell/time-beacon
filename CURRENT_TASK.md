# Task: Clean Up MeshCore Code and Archive Technical Specification

## Task Goal
Clean up experimental MeshCore code from firmware, archive full technical specifications and client firmware patch architecture in `docs/meshcore_integration_spec.md`, and streamline the LoRa beacon service to focused 2-phase Meshtastic rotation (LongFast and MediumFast alternating every minute):
1. In `docs/meshcore_integration_spec.md`:
   - Create comprehensive reference document detailing MeshCore packet wire format, Ed25519 authentication structure, why stock MeshCore nodes ignore ambient time broadcasts, client-side modification proposal for remote trusted Stratum-1 time sync, and Public channel group text alternative.
2. In `main/include/config_mgr.h`:
   - Remove `enable_meshcore` field from `app_config_t`.
3. In `main/config_mgr.c`:
   - Remove `enable_meshcore` initialization, INI parsing, and default config writing.
4. In `main/lora_time_beacons.c`:
   - Remove `build_meshcore_packet()`, `send_meshcore_beacon()`, and `get_meshcore_freq()`.
   - Simplify beacon task loop to pure 2-phase Meshtastic rotation (even minutes = LongFast, odd minutes = MediumFast).
   - Clean up startup pulses to send LongFast and MediumFast.
5. In `main/main.c`:
   - Update LoRa service start check to `cfg->enable_meshtastic`.
6. In `tests/test_harness.c`:
   - Remove obsolete `test_meshcore_packet_layout()`.
   - Update `test_lora_rotation_schedule()` to verify 2-phase rotation (0 = LongFast, 1 = MedFast).
   - Remove `meshcore` test artifacts from config parsing tests.
7. Verification command:
   ```bash
   ./build_tracker.sh all && ./tests/run_tests.sh
   ```
8. Verify build and tests pass with zero errors and zero warnings, mark task complete, and auto-commit.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `docs/meshcore_integration_spec.md`
- `main/include/config_mgr.h`
- `main/config_mgr.c`
- `main/lora_time_beacons.c`
- `main/main.c`
- `tests/test_harness.c`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Document MeshCore integration and remote-trust architecture in `docs/meshcore_integration_spec.md`.
- [x] Remove `enable_meshcore` from `main/include/config_mgr.h` and `main/config_mgr.c`.
- [x] Clean up MeshCore code from `main/lora_time_beacons.c` and streamline to 2-phase Meshtastic rotation.
- [x] Update `main/main.c`.
- [x] Update test suite in `tests/test_harness.c`.
- [x] Verify build and tests pass with zero errors and zero warnings.
- [x] Automatically commit verified changes per AGENTS.md.
