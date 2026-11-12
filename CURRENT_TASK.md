# Task: Dynamic Radio Power Scaling and User-Configurable Screen Orientation

## Task Goal
1. Dynamic Radio Power Scaling:
   - In `main/include/wifi_manager.h` and `main/wifi_manager.c`:
     - Implement `void wifi_manager_update_power(bool usb_connected, float battery_voltage)`.
     - When USB is connected or battery is healthy ($\ge 3.75\text{V}$), scale Wi-Fi TX power up to 18 dBm (`esp_wifi_set_max_tx_power(72)`) for long range.
     - When running on depleted battery ($< 3.55\text{V}$), scale Wi-Fi TX power down to 13 dBm (`esp_wifi_set_max_tx_power(52)`) to protect against brownout voltage sags.
   - In `main/display.c`:
     - In `display_refresh_task()`, call `wifi_manager_update_power()` on each status tick (~1s).
2. User-Configurable Screen Orientation (0° / 180° flip):
   - In `main/include/config_mgr.h`:
     - Add `bool display_flip` to `app_config_t`.
   - In `main/config_mgr.c`:
     - Support `[display] flip = true/false` in `config.ini`.
     - Expose `esp_err_t config_mgr_set_display_flip(bool flip)` to persist to NVS/config.
   - In `main/include/display.h` and `main/display.c`:
     - Support 180° orientation flipping dynamically via `display_set_flip(bool flip)`.
     - On user button press while screen is awake:
       - Debounce and detect click.
       - Toggle display flip between normal (`0x68`) and inverted (`0xA8`).
       - Persist flip state so it survives reboot.
3. In `tests/test_harness.c`:
   - Add unit tests for `display_flip` configuration parsing and dynamic TX power calculation.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `main/include/config_mgr.h`
- `main/config_mgr.c`
- `main/include/wifi_manager.h`
- `main/wifi_manager.c`
- `main/include/display.h`
- `main/display.c`
- `tests/test_harness.c`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Add `display_flip` to `config_mgr.h` and `config_mgr.c`.
- [x] Implement `display_set_flip()` and button toggle in `display.c`.
- [x] Implement `wifi_manager_update_power()` in `wifi_manager.c`.
- [x] Add unit test coverage in `tests/test_harness.c`.
- [x] Verify build and tests pass with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
