# Task: Restore USB Serial JTAG PHY on Exiting USB Flash Drive Mode

## Task Goal
Fix USB Serial/JTAG disappearing and failing to re-enumerate after exiting USB Flash Drive mode:
1. Cause:
   In `main/usb_msc.c`, when the user exits Config Mode via button tap, `esp_restart()` is called immediately while TinyUSB holds the USB PHY in USB-OTG mode.
   On ESP32-S3 software restart, the internal USB PHY routing does not reset back to the hardware USB Serial/JTAG controller, leaving USB Serial/JTAG un-enumerated and invisible to `usb_serial_jtag_is_connected()` and `idf.py monitor`.
2. Fix:
   In `main/usb_msc.c`:
   - De-register / delete TinyUSB storage (`tinyusb_msc_delete_storage`).
   - Uninstall TinyUSB driver (`tinyusb_driver_uninstall`).
   - Unmount wear levelling (`wl_unmount`).
   - Re-initialize internal USB PHY to hardware USB Serial/JTAG controller via `usb_new_phy(&phy_conf, &jtag_phy)`.
   - Delay briefly for USB bus reset / host disconnection before `esp_restart()`.
3. Verify test harness and tracker builds.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `main/usb_msc.c`
- `tests/test_harness.c`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Cleanly teardown TinyUSB and restore USB Serial/JTAG PHY in `usb_msc.c`.
- [x] Verify build and tests pass with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
