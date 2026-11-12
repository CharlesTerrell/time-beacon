# Task: Disable Brownout Detector and Enable PHY Calibration Storage

## Task Goal
Prevent brownout resets during combined battery charging and Wi-Fi PHY calibration:
1. In `sdkconfig.defaults`:
   - Disable hardware brownout reset detector (`CONFIG_ESP_BROWNOUT_DET=n`).
   - Enable PHY calibration data storage (`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE=y`) to prevent heavy analog RF recalibration on boot.
   - Use partial PHY calibration (`CONFIG_ESP_PHY_INIT_DATA_IN_PARTITION=y`).
2. Verification command:
   ```bash
   ./build_tracker.sh all && ./tests/run_tests.sh
   ```
3. Auto-commit on clean build and tests.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `sdkconfig.defaults`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Update `sdkconfig.defaults` to disable BOD and enable PHY calibration storage.
- [x] Verify build and tests pass with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
