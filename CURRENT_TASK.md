# Task: Update Power Management and Display Docs for Current Firmware Behavior

## Task Goal
Update `docs/plan/power_management.md` and `docs/plan/display_management.md` to document:
1. Dynamic Wi-Fi TX power scaling (18 dBm on USB/healthy battery, throttled to 13 dBm when battery $< 3.65\text{V}$ to protect the onboard TP4054 charger).
2. Battery priority rule: Low battery state and power throttling always take precedence over USB presence.
3. USB Host vs. Wall Charger behavior (`usb_serial_jtag_is_connected()` vs. terminal voltage jump under charging load).
4. Early boot brownout guard and 60-second deep sleep recovery cycle.
5. Critical battery shutdown countdown cancellation when USB is plugged in, and re-arming when unplugged.
6. User-configurable and runtime 180° screen orientation flipping via `USER` button and `[display] flip` in `config.ini`.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `docs/plan/power_management.md`
- `docs/plan/display_management.md`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Update `docs/plan/power_management.md`.
- [x] Update `docs/plan/display_management.md`.
- [x] Verify build and tests pass with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
