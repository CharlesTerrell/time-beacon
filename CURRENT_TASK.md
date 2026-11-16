# Task: Update All Plan Docs to Reflect Current C Code Implementation

## Task Goal
Update all remaining plan documentation in `./docs/plan/` to reflect current firmware implementation:
1. `docs/plan/boot_selector.md`:
   - Document 2.5s boot prompt for USB MSC Flash Drive Config Mode (`config.ini` on FATFS partition).
   - Document clean TinyUSB teardown and USB Serial/JTAG PHY restoration on exit.
   - Document direct Stratum-1 boot on prompt timeout.
2. `docs/plan/meshtastic_beacons.md`:
   - Correct US915 MediumFast frequency to 913.125 MHz (slot 45).
   - Correct channel hashes: `0x08` (LongFast) and `0x1F` (MediumFast), hop_limit/hop_start `0x63`.
   - Document 3-packet rotation: `POSITION_APP` (with precision bits), `NODEINFO_APP`, and `TELEMETRY_APP`.
   - Note MeshCore dead code removal per commit `5cd5bec`.
3. `docs/plan/wifi_ntp.md`:
   - Clarify Station mode (`WIFI_MODE_STA`) operation with DHCP/Static IP from `config.ini`.
   - Document dynamic Wi-Fi TX power scaling (13 / 15 / 18 dBm) and 15 dBm startup cap.
   - Document modem sleep and regulatory country code configuration.
4. `docs/plan/gps_time_position.md`:
   - Replace legacy `gps_mgr.h` references with actual architecture (`gnss_driver.h`, `time_sync.h`, `gnss_task.h`, `nmea_parser.h`).
   - Document hardware PPS routing differences: Tracker v1.1 unrouted PPS (`BOARD_HAS_GNSS_PPS 0`) vs Tracker v2 / WiFi LoRa 32 V4 hardware GPIO PPS (`BOARD_HAS_GNSS_PPS 1`).
   - Document USB NMEA streaming output (`usb_nmea_output`).
5. `docs/plan/display_management.md`:
   - Add multi-board display support: ST7735 80x160 SPI TFT (Tracker v1.1/v2) and SSD1306 128x64 I2C OLED (WiFi LoRa 32 V4).
   - Document status line `X.XV*` USB charging battery format.
   - Document persistent 180° orientation flip via `config_mgr_set_display_flip()` and `[display] flip` in `config.ini`.
6. `docs/plan/ble_cts.md`:
   - Document standard Battery Service (BAS UUID `0x180F`) support controlled by `enable_ble_battery`.
7. Run verification command `./tests/run_tests.sh` with zero errors and commit.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `docs/plan/boot_selector.md`
- `docs/plan/meshtastic_beacons.md`
- `docs/plan/wifi_ntp.md`
- `docs/plan/gps_time_position.md`
- `docs/plan/display_management.md`
- `docs/plan/ble_cts.md`

## Verification Command
```bash
./tests/run_tests.sh
```

## Checklist
- [x] Update `docs/plan/boot_selector.md`.
- [x] Update `docs/plan/meshtastic_beacons.md`.
- [x] Update `docs/plan/wifi_ntp.md`.
- [x] Update `docs/plan/gps_time_position.md`.
- [x] Update `docs/plan/display_management.md`.
- [x] Update `docs/plan/ble_cts.md`.
- [x] Run verification command with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
