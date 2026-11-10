# Rules for AI Agent (Qwen + Aider)

## Core Contract
- Target: ESP32-S3 (`esp32s3`), ESP-IDF v5.x (`-std=gnu17`).
- Edit only files listed in `CURRENT_TASK.md`.
- Read only `CURRENT_TASK.md` for task goals. Never read `TODO.md` unless instructed.
- When task verification command passes with zero errors and zero warnings:
  1. Mark task `[x]` in `CURRENT_TASK.md`.
  2. Automatically stage and commit verified changes with Conventional Commits message and `Co-authored-by: Antigravity (Google Gemini) <antigravity@google.com>`.

## Code Quality Rules
- Fallible functions return `esp_err_t`. Use standard IDF errors (`ESP_OK`, `ESP_ERR_INVALID_ARG`, `ESP_FAIL`).
- `ESP_ERROR_CHECK()` allowed only in `app_main()` or component initialization. Use `if (err != ESP_OK)` in runtime loops.
- Static memory only: No `malloc` or heap calls inside packet loops, task loops, or ISRs.
- Always use `ESP_LOGI`, `ESP_LOGW`, `ESP_LOGE` with `static const char *TAG`. No raw `printf`.
- Types: Fixed-width `<stdint.h>` (`uint8_t`, `uint32_t`, etc.) and `stdbool.h`.
- Hardware pins: Use constants from `main/include/board_pins.h`. Never hardcode raw GPIO numbers.
- Network byte order: Convert all multi-byte NTP fields with `htonl()` / `htons()`.
