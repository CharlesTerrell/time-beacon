#!/usr/bin/env bash
# Source this file to activate the ESP-IDF environment for Time Beacon:
#   source tools/idf_env.sh

# Fast path: already active
if command -v idf.py >/dev/null 2>&1; then
    return 0 2>/dev/null || exit 0
fi

# Direct path export: Instant, no python subprocess hanging
export IDF_PATH="${IDF_PATH:-/home/charlie/.espressif/v6.1/esp-idf}"
export ESP_IDF_VERSION="6.1"
export OPENOCD_SCRIPTS="/home/charlie/.espressif/tools/openocd-esp32/v0.12.0-esp32-20260703/openocd-esp32/share/openocd/scripts"
export ESP_ROM_ELF_DIR="/home/charlie/.espressif/tools/esp-rom-elfs/20241011/"
export IDF_PYTHON_ENV_PATH="/home/charlie/.espressif/python_env/idf6.1_py3.14_env"
export PATH="/home/charlie/.espressif/tools/xtensa-esp-elf-gdb/17.1_20260402/xtensa-esp-elf-gdb/bin:/home/charlie/.espressif/tools/riscv32-esp-elf-gdb/17.1_20260402/riscv32-esp-elf-gdb/bin:/home/charlie/.espressif/tools/xtensa-esp-elf/esp-15.2.0_20251204/xtensa-esp-elf/bin:/home/charlie/.espressif/tools/esp-clang/esp-21.1.3_20260408/esp-clang/bin:/home/charlie/.espressif/tools/riscv32-esp-elf/esp-15.2.0_20251204/riscv32-esp-elf/bin:/home/charlie/.espressif/tools/esp32ulp-elf/2.38_20240113/esp32ulp-elf/bin:/home/charlie/.espressif/tools/openocd-esp32/v0.12.0-esp32-20260703/openocd-esp32/bin:/home/charlie/.espressif/tools/ninja/1.12.1/:/home/charlie/.espressif/tools/esp-clangd/esp-21.1.3_20260408/esp-clangd/bin:/home/charlie/.espressif/python_env/idf6.1_py3.14_env/bin:/home/charlie/.espressif/v6.1/esp-idf/tools:$PATH"

if command -v idf.py >/dev/null 2>&1; then
    echo "ESP-IDF activated: $(command -v idf.py)"
fi
