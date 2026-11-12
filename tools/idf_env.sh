#!/usr/bin/env bash
# Source this file to activate the ESP-IDF environment for Time Beacon:
#   source tools/idf_env.sh

# Fast path: Use cached environment file if present and valid
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
if [ -f "$SCRIPT_DIR/.env.esp" ]; then
    . "$SCRIPT_DIR/.env.esp"
    if command -v idf.py >/dev/null 2>&1; then
        echo "ESP-IDF activated from cached .env.esp: $(command -v idf.py)"
        return 0 2>/dev/null || exit 0
    fi
fi
    # 1. Already in PATH
    if command -v idf.py >/dev/null 2>&1; then
        return 0
    fi

    # 2. IDF_PATH already set in environment
    if [ -n "${IDF_PATH:-}" ] && [ -f "$IDF_PATH/export.sh" ]; then
        echo "$IDF_PATH/export.sh"
        return 0
    fi

    # 3. Standard Espressif v6.x or v5.x directory
    for p in "$HOME/.espressif/v6."*/esp-idf/export.sh \
             "$HOME/.espressif/v5."*/esp-idf/export.sh \
             "$HOME/esp-idf/export.sh" \
             "$HOME/esp/esp-idf/export.sh" \
             "/opt/esp-idf/export.sh"; do
        if [ -f "$p" ]; then
            echo "$p"
            return 0
        fi
    done

    return 1
}

if command -v idf.py >/dev/null 2>&1; then
    echo "ESP-IDF already active: $(command -v idf.py)"
else
    EXPORT_PATH="$(_find_idf_export || true)"
    if [ -n "$EXPORT_PATH" ] && [ -f "$EXPORT_PATH" ]; then
        echo "Activating ESP-IDF from $EXPORT_PATH..."
        # Source without polluting shell options
        . "$EXPORT_PATH"
    else
        echo "Error: Could not locate ESP-IDF export.sh." >&2
        echo "Please set IDF_PATH or install ESP-IDF v5.x/v6.x." >&2
        return 1 2>/dev/null || exit 1
    fi
fi
