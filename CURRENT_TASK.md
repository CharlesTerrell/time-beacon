# Task: Prioritize Battery Protection Over USB Status in Power Management

## Task Goal
Prevent brownout resets when running with a low battery connected to USB:
1. In `main/wifi_manager.c` & `main/include/wifi_manager.h`:
   - Update `wifi_manager_update_power(bool usb_connected, bool battery_present, float battery_voltage)`.
   - Priority rule:
     - If physical battery is present and $< 3.65\text{V}$, limit Wi-Fi TX power to 13 dBm (`esp_wifi_set_max_tx_power(52)`), regardless of USB connection, to leave maximum current budget for the onboard charger.
     - Only boost to 18 dBm if:
       - No battery is present (`!battery_present`), OR
       - Battery is healthy ($\ge 3.75\text{V}$).
2. In `main/power_mgr.c`:
   - In `power_mgr_early_battery_guard()`:
     - Check battery voltage first!
     - If battery is present and $\le 3.25\text{V}$ (deeply discharged / in pre-charge state), do NOT bypass guard simply because USB is connected. If battery is $\le 3.25\text{V}$, enter low-power pre-charge sleep (60s timer) to let TP4054 charge the cell without the CPU/Wi-Fi drawing power.
   - In `power_mgr_update()`:
     - If battery is present and $\le \text{VBAT\_CRITICAL\_V}$, mark mode `POWER_MODE_BATTERY_CRITICAL` even if USB is detected, warning user and preserving power until cell reaches safe operating voltage.
3. In `main/display.c`:
   - Pass `pwr.battery_present` to `wifi_manager_update_power()`.
4. In `tests/test_harness.c`:
   - Update unit tests to verify battery voltage overrides USB status when battery is present.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `main/include/wifi_manager.h`
- `main/wifi_manager.c`
- `main/power_mgr.c`
- `main/display.c`
- `tests/test_harness.c`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Update `power_mgr.c` so low battery condition is never masked by USB.
- [x] Update `wifi_manager.c` and `wifi_manager.h` power scaling rules.
- [x] Update `display.c` caller.
- [x] Update unit tests in `tests/test_harness.c`.
- [x] Verify build and tests pass with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
