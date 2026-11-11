# Task: De-Brand Generic Heltec References to Time Beacon

## Task Goal
Remove generic "Heltec" branding where not referring to actual board hardware:
1. `deploy/chrony.conf`: Update comments from "Heltec Wireless Tracker v1.1" to "Time Beacon Stratum 1 GPS NTP Appliance".
2. `deploy/test_ntp.sh`: Update echo banners and test labels from "Heltec NTP Server" to "Time Beacon NTP Server".
3. `main/config_mgr.c`:
   - Change default `node_name` from `"Heltec-Time"` to `"TimeBeacon"`.
   - Update config file header comment to `"# Time Beacon Stratum-1 Server Configuration\n"`.
4. `main/ble_cts_service.c`:
   - Update default `s_device_name` to `"TimeBeacon"`.
5. `main/display.c`:
   - Update title headers from `"HELTEC TIME STRATUM 1"` and `"HELTEC NTP SERVER"` to `"TIME BEACON STRATUM 1"` and `"TIME BEACON NTP"`.
6. `main/Kconfig.projbuild`:
   - Change menu `"Heltec NTP Configuration"` to `"Time Beacon Configuration"`.
7. `tests/test_harness.c`:
   - Update test `node_name` default to `"TimeBeacon"`.
8. Retain legitimate board-specific hardware strings:
   - Board definitions (`BOARD_HARDWARE_NAME`, `CONFIG_BOARD_HELTEC_V1_1`, `CONFIG_BOARD_HELTEC_V2`).
   - Meshtastic `hw_model = 48` (`HELTEC_WIRELESS_TRACKER` enum).
   - USB VID/PID manufacturer descriptor where applicable.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `deploy/chrony.conf`
- `deploy/test_ntp.sh`
- `main/config_mgr.c`
- `main/ble_cts_service.c`
- `main/display.c`
- `main/Kconfig.projbuild`
- `tests/test_harness.c`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Update `deploy/chrony.conf` and `deploy/test_ntp.sh`.
- [x] Update default names and banner strings in `main/config_mgr.c`, `main/ble_cts_service.c`, and `main/display.c`.
- [x] Update `main/Kconfig.projbuild`.
- [x] Update `tests/test_harness.c`.
- [x] Verify build and tests pass with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
