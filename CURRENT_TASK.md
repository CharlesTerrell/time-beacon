# Task: Eliminate Brownout Crashes by Tuning Wi-Fi Power and BOD Threshold

## Task Goal
Prevent brownout resets when initializing Wi-Fi and connecting:
1. In `main/wifi_manager.c`:
   - Reduce maximum Wi-Fi TX power from maximum 20 dBm (80 units, 450mA peak) to a conservative 15 dBm (60 units) using `esp_wifi_set_max_tx_power(60)`.
   - Set power save mode to `WIFI_PS_MIN_MODEM` on startup instead of `WIFI_PS_NONE` to mitigate RF current spikes during station association.
2. In `sdkconfig.defaults`:
   - Set brownout detector level to `CONFIG_ESP_BROWNOUT_DET_LVL_SEL_4=y` (approx 2.65V instead of level 7 ~ 2.85V).
3. In `tools/idf_env.sh`:
   - Add fast-path check for cached `.env.esp` to avoid slow Python export on every shell invocation.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `main/wifi_manager.c`
- `sdkconfig.defaults`
- `tools/idf_env.sh`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Configure Wi-Fi TX power cap and modem sleep in `main/wifi_manager.c`.
- [x] Adjust BOD threshold in `sdkconfig.defaults`.
- [x] Add fast cached environment loader to `tools/idf_env.sh`.
- [x] Verify build and tests pass with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
