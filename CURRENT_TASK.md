# Task: Dynamic Emergency Sleep Countdown Management with USB Charging

## Task Goal
Properly handle plugging and unplugging USB while in critical battery state:
1. In `main/power_mgr.c`:
   - When in critical battery state ($\text{voltage} \le 3.35\text{V}$):
     - If running on battery (`!usb_connected`):
       - If countdown is not running, start 30s emergency sleep countdown (`critical_countdown_sec = 30`).
       - Countdown decrements each second until 0 $\to$ deep sleep.
     - If USB is plugged in (`usb_connected == true`):
       - Cancel and disarm the emergency sleep countdown (`critical_countdown_sec = -1`).
       - Keep `mode = POWER_MODE_BATTERY_CRITICAL` (or charging warning), maintaining throttled Wi-Fi power (13 dBm) so cell can charge.
     - If subsequently unplugged from USB while battery is still $\le 3.35\text{V}$:
       - Re-arm the 30-second emergency sleep countdown immediately (`critical_countdown_sec = 30`).
2. In `main/display.c`:
   - In `display_update()` / status formatting:
     - When `mode == POWER_MODE_BATTERY_CRITICAL` and USB is connected, indicate charging status cleanly (e.g. `[CHG]`).
3. In `tests/test_harness.c`:
   - Add unit tests verifying:
     - Critical battery on battery starts 30s countdown.
     - Connecting USB disarms countdown to -1.
     - Disconnecting USB while still critical re-arms countdown to 30s.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `main/power_mgr.c`
- `main/display.c`
- `tests/test_harness.c`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Implement USB disarm and re-arm logic in `power_mgr.c`.
- [x] Update display telemetry indicators in `display.c`.
- [x] Add unit test coverage in `tests/test_harness.c`.
- [x] Verify build and tests pass with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
