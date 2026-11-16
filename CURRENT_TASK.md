# Task: Update README.md to Reflect Current Project Architecture

## Task Goal
Update `README.md` to reflect current project capabilities, configuration mechanisms, hardware support, and memory characteristics:
1. Fix typo and update protocol summary (Station mode Wi-Fi with dynamic TX throttling, BLE CTS + BAS, 2-phase Meshtastic with 3-packet rotation, USB MSC flash drive configuration).
2. Update supported hardware list: Heltec Wireless Tracker v1.1, Tracker v2, and WiFi LoRa 32 V4.
3. Replace legacy `time_beacon.ini` snippet with current `/storage/config.ini` schema.
4. Document USB Mass Storage configuration mode (hold `USER` during 2.5s boot prompt).
5. Add RAM and Flash memory footprint summary.
6. Verify via `./tests/run_tests.sh` and commit.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `README.md`

## Verification Command
```bash
./tests/run_tests.sh
```

## Checklist
- [x] Update `README.md` with accurate architecture, hardware list, configuration, and memory stats.
- [x] Run verification command with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
