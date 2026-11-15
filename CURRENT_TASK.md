# Task: Eliminate Brownout Detector Boot Loops and Throttle Charging Wi-Fi Power

## Task Goal
1. Fix Hardware Brownout Detector Reset Loops:
   - In `sdkconfig.defaults`:
     - Explicitly set `# CONFIG_ESP_BROWNOUT_DET is not set` to prevent hardware brownout reset during USB power transients, battery charging, and RF spikes.
   - In `main/main.c`:
     - Call `esp_brownout_disable()` at the very start of `app_main()` as a runtime defense against boot brownouts.
2. Fix Current Overload during Low-Battery USB Charging:
   - In `main/wifi_manager.c`:
     - When USB is connected AND a battery is present and charging (`battery_voltage < 4.10f`), cap Wi-Fi TX power to 15 dBm (60 units) or 13 dBm (52 units if $<3.65\text{V}$).
     - Do not allow Wi-Fi to jump to 18 dBm (72 units) while battery is actively drawing 500mA charging current, preventing 5V rail sag and brownouts on 500mA USB ports.
3. Unit tests in `tests/test_harness.c`:
   - Update Wi-Fi TX power scaling tests to verify 15 dBm cap while battery is charging on USB, and 18 dBm allowed only when battery is absent or fully charged.
4. Verification command:
   ```bash
   ./build_tracker.sh all && ./tests/run_tests.sh
   ```
5. Auto-commit on clean build and tests per `AGENTS.md`.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `sdkconfig.defaults`
- `main/main.c`
- `main/wifi_manager.c`
- `tests/test_harness.c`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Disable brownout detector in `sdkconfig.defaults`.
- [x] Call `esp_brownout_disable()` at early `app_main()` in `main/main.c`.
- [x] Throttle Wi-Fi TX power during battery charging in `main/wifi_manager.c`.
- [x] Update unit test coverage in `tests/test_harness.c`.
- [x] Run verification command with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
