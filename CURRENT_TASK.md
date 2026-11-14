# Task: Fix Persistent Screen Flip via config.ini

## Task Goal
Fix `[display] flip` in `config.ini` not taking effect:
1. Cause: `config_mgr_init()` mounts FATFS and loads `config.ini` in `app_main()`, but `display_init()` is called BEFORE `config_mgr_init()`.
   When `display_init()` calls `config_mgr_get()`, `s_config` has only compiled defaults (`display_flip = false`).
2. Fix:
   - Call `display_set_flip(cfg->display_flip)` in `app_main()` immediately after `config_mgr_init()` loads `config.ini`.
   - In `display_set_flip()`, re-apply `MADCTL` register (`0x36`) and invalidate/refresh frame if needed.
   - Also, in `config_mgr_set_display_flip(bool flip)`, if persistent write back to `config.ini` or NVS is desired, update `s_config.display_flip`.
3. Verify test harness and tracker builds.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `main/main.c`
- `main/display.c`
- `main/include/display.h`
- `tests/test_harness.c`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Update `app_main()` to apply `cfg->display_flip` after `config_mgr_init()`.
- [x] Verify `display_set_flip()` correctly updates `s_flipped` and sends `MADCTL` to display.
- [x] Run verification command with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
