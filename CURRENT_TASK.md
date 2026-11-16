# Task: Update Power Management Documentation to Reflect Current C Code

## Task Goal
Update `./docs/plan/power_management.md` to align with the active implementation across `power_mgr.c`, `wifi_manager.c`, `main.c`, `display.c`, `board_pins.h`, and `sdkconfig.defaults`:
1. Hardware interface:
   - Clarify `PIN_VBAT_CTRL` active HIGH polarity (NPN Q3 driving P-MOSFET Q2 gate LOW).
   - Document board pin mappings across Heltec Wireless Tracker v1.1, v2, and WiFi LoRa 32 V4.
   - Detail brownout prevention: `# CONFIG_ESP_BROWNOUT_DET is not set` in Kconfig and early `esp_brownout_disable()` in `app_main()`.
   - Detail LEDC PWM breathing pulse and critical battery flash on `PIN_STATUS_LED`.
2. USB & Battery detection:
   - Document sample variance ($>200$) and voltage ($<1.0\text{V}$) open battery port detection.
   - Document wall charger float voltage detection ($>4.18\text{V}$).
3. Dynamic Wi-Fi TX power scaling:
   - 13 dBm (52 units) for low battery ($<3.65\text{V}$).
   - 15 dBm (60 units) cap while actively charging on USB ($<4.10\text{V}$) to avoid overloading 500mA USB ports.
   - 18 dBm (72 units) on pure USB or fully charged battery ($\ge 4.10\text{V}$ USB, $\ge 3.75\text{V}$ battery).
4. Sleep & Hysteresis:
   - Early boot guard at $\le 3.25\text{V}$ (bypassed if USB plugged in or battery missing).
   - 30-second runtime countdown at $\le 3.35\text{V}$ with $3.50\text{V}$ recovery hysteresis latch.
5. Telemetry & Display theming:
   - Status bar indicators (`USB`, `X.XV*`, `[CHG] X.XV`, `LOW BAT! SLEEP:Xs`).
   - Adaptive RGB565 color transitions (White on USB, Red on critical, Cyan to Yellow gradient on battery).
   - BLE CTS and LoRa Meshtastic (101% on USB) telemetry.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `docs/plan/power_management.md`

## Verification Command
```bash
./tests/run_tests.sh
```

## Checklist
- [x] Update `docs/plan/power_management.md` with hardware architecture, active HIGH divider gate, and brownout protection.
- [x] Document dynamic Wi-Fi power throttling during USB battery charging.
- [x] Document early boot guard, 30s emergency sleep countdown, and 3.50V bounce hysteresis latch.
- [x] Document LED PWM breathing pulse, adaptive display color theming, and remote telemetry broadcasting.
- [x] Run verification command with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
