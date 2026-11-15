# Task: Fix Low Battery Countdown Hysteresis and Bootloader Charge Loop

## Task Goal
1. Low Battery Shutdown Countdown Hysteresis:
   - In `main/power_mgr.c`:
     - Once `POWER_MODE_BATTERY_CRITICAL` countdown arms ($\le 3.35\text{V}$), do NOT cancel on cell bounce.
     - Latch critical mode until USB is connected or voltage recovers above $3.50\text{V}$ (`VBAT_CRITICAL_RECOVERY_V`).
2. Fix Depleted Battery Boot Loop when Plugged into USB:
   - In `main/power_mgr.c` (`power_mgr_early_battery_guard`):
     - If USB power is detected (either via `usb_serial_jtag_is_connected()` or voltage jump), do NOT plunge into 60s deep sleep. Instead, allow device to boot in low-power charging mode, avoiding sleep loops.
     - Before entering deep sleep on battery, wait until `PIN_USER_BTN` is released and debounce to avoid immediate `ext0` wakeup loop.
     - Enable RTC pull-up on `PIN_USER_BTN` during sleep so floating button line does not false-trigger wakeup.
3. Unit tests in `tests/test_harness.c`:
   - Test countdown latching hysteresis (voltage bouncing from 3.33V to 3.42V does not disarm countdown; USB or >3.50V disarms).
4. Verification command:
   ```bash
   ./build_tracker.sh all && ./tests/run_tests.sh
   ```
5. Auto-commit on clean build and tests per `AGENTS.md`.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `main/power_mgr.c`
- `tests/test_harness.c`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Add voltage hysteresis to critical battery countdown in `main/power_mgr.c`.
- [x] Fix USB presence and button release in `power_mgr_early_battery_guard()`.
- [x] Add unit test coverage in `tests/test_harness.c`.
- [x] Run verification command with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
