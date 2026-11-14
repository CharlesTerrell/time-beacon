# Task: Fix Battery ADC Control Polarity and Display Battery Voltage

## Task Goal
Fix battery detection stuck on `USB` mode and display voltage clearly:
1. In `main/power_mgr.c`:
   - Change `PIN_VBAT_CTRL` (GPIO 2) polarity from active LOW to active HIGH:
     - Initialization / idle: `gpio_set_level(PIN_VBAT_CTRL, 0);` (Q3 NPN off)
     - Sampling: `gpio_set_level(PIN_VBAT_CTRL, 1);` (Q3 NPN on -> Q2 P-MOSFET gate low -> divider connected)
     - Post-sampling: `gpio_set_level(PIN_VBAT_CTRL, 0);`
2. In `main/display.c`:
   - Display battery voltage in all power modes on line 5:
     - On USB with battery: `N:<p> L:<l> <bt> 4.1V*`
     - On USB without battery: `N:<p> L:<l> <bt> USB`
     - On Battery: `N:<p> L:<l> <bt> 3.8V`
     - Critical battery on battery: `LOW BAT! SLEEP:<sec>s`
3. Update `tests/test_harness.c` to test battery status and display formatting.
4. Auto-commit on clean build and tests.

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
- [x] Fix `PIN_VBAT_CTRL` active HIGH polarity in `power_mgr.c`.
- [x] Update display line 5 battery voltage readout across all modes in `display.c`.
- [x] Update tests in `tests/test_harness.c`.
- [x] Run verification command with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
