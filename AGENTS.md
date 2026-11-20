# Rules for AI Agents

## Core Contract
- Target: ESP32-S3 (`esp32s3`), ESP-IDF v5.x and v6.x (`-std=gnu17` / `-std=gnu23`).
- Edit only files listed in `CURRENT_TASK.md`.
- Read only `CURRENT_TASK.md` for task goals.
- When task verification command passes with zero errors and zero warnings:
  1. Mark task `[x]` in `CURRENT_TASK.md`.
  2. Automatically stage and commit verified changes with Conventional Commits message and `Co-authored-by:` trailer matching the active agent tool and model (e.g., Antigravity/Gemini, Aider/Qwen, Claude Code).

## Recommended CLI & Inspection Tools
- Build and flash script: `./build.sh`
- Serial console & logging: `tio -b 115200 /dev/ttyACM0` or `idf.py monitor`.
- Fast search: `rg` (ripgrep) for searching symbol definitions, registers, and strings across the codebase.
- Symbol indexing: `ctags -R .` or `cscope -b -R` for jumping to C struct, enum, and function definitions.
- Binary & compilation database: `bear` or `ninja -t compdb` for generating `compile_commands.json`.
- Static analysis: `clang-tidy` (using `compile_commands.json`) to catch type mismatches and memory safety bugs.
- Footprint & memory audit: `./build.sh size [v1.1|v2] [components]` or `idf.py size` and `idf.py size-components` to track flash, DRAM, and IRAM usage.
- ELF symbol size inspection: `xtensa-esp32s3-elf-nm -S --size-sort build_v1/*.elf` to locate oversized buffers or functions.

## Code Quality Rules
- Fallible functions return `esp_err_t`. Use standard IDF errors (`ESP_OK`, `ESP_ERR_INVALID_ARG`, `ESP_FAIL`).
- `ESP_ERROR_CHECK()` allowed only in `app_main()` or component initialization. Use `if (err != ESP_OK)` in runtime loops.
- Static memory only: No `malloc` or heap calls inside packet loops, task loops, or ISRs.
- Always use `ESP_LOGI`, `ESP_LOGW`, `ESP_LOGE` with `static const char *TAG`. No raw `printf`.
- Types: Fixed-width `<stdint.h>` (`uint8_t`, `uint32_t`, etc.) and `stdbool.h`.
- Hardware pins: Use constants from `main/include/board_pins.h`. Never hardcode raw GPIO numbers.
- Network byte order: Convert all multi-byte NTP fields with `htonl()` / `htons()`.

