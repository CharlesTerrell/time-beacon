# Task: Fix usb component requirement for ESP-IDF v5.x in main/CMakeLists.txt

## Task Goal
Resolve `fatal error: esp_private/usb_phy.h: No such file or directory` under ESP-IDF v5.x (CI container v5.3.2) by conditionally requiring the `usb` component when `IDF_VERSION_MAJOR` is less than 6, while maintaining compatibility with ESP-IDF v6.x.

## Allowed Files to Edit
- CURRENT_TASK.md
- main/CMakeLists.txt

## Verification Command
```bash
./build.sh test && ./build.sh v1.1
```

## Checklist
- [x] Conditionally add `usb` to `REQUIRES` in `main/CMakeLists.txt` for ESP-IDF v5.x.
- [x] Run verification command with zero errors and zero warnings.
- [x] Mark checklist items complete and commit changes per AGENTS.md rules.
