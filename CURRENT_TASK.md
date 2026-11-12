# Task: Add Early Boot Low-Battery Brownout Guard

## Task Goal
Prevent low-battery brownout bootloops when booting on depleted LiPo cell:
1. In `main/include/power_mgr.h`:
   - Declare `esp_err_t power_mgr_early_battery_guard(void)`.
2. In `main/power_mgr.c`:
   - Implement `power_mgr_early_battery_guard()`:
     - Check if USB is connected or battery missing. If so, return `ESP_OK`.
     - Read battery voltage. If $\le 3.30\text{V}$ (critical brownout threshold):
       - Show brief warning message on display.
       - Configure timer wakeup (60s) or button wakeup.
       - Enter deep sleep immediately before any high-current radio initialization.
3. In `main/main.c`:
   - Call `power_mgr_early_battery_guard()` immediately after `power_mgr_init()` and before 2.5s countdown or radio init.
4. In `tests/test_harness.c`:
   - Add unit test for critical voltage detection logic.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `main/include/power_mgr.h`
- `main/power_mgr.c`
- `main/main.c`
- `tests/test_harness.c`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Implement `power_mgr_early_battery_guard()` in `power_mgr.h` and `power_mgr.c`.
- [x] Integrate early boot guard into `main/main.c`.
- [x] Add test coverage in `tests/test_harness.c`.
- [x] Verify build and tests pass with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
