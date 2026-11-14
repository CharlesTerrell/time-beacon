#!/usr/bin/env bash
# Unified ESP-IDF Environment Manager for Time Beacon
#
# Usage:
#   source tools/idf_env.sh            # Activate environment in current shell (fast cached)
#   ./tools/idf_env.sh [--refresh]     # Refresh / regenerate cached .env.esp

# Determine whether this script is being sourced or executed directly
_IS_SOURCED=0
if [[ "${BASH_SOURCE[0]}" != "${0}" ]]; then
    _IS_SOURCED=1
fi

# Locate workspace root directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ENV_CACHE="$SCRIPT_DIR/.env.esp"

# Fast path for sourced usage: if idf.py is already active in current shell, exit immediately
if [ "$_IS_SOURCED" -eq 1 ] && [ "${1:-}" != "--refresh" ] && [ "${1:-}" != "-f" ]; then
    if command -v idf.py >/dev/null 2>&1; then
        return 0 2>/dev/null || exit 0
    fi
fi

# Discover local ESP-IDF installation if IDF_PATH is not set or invalid
if [ -z "${IDF_PATH:-}" ] || [ ! -f "$IDF_PATH/tools/idf_tools.py" ]; then
    for candidate in $(ls -d "$HOME"/.espressif/v*/esp-idf 2>/dev/null | sort -V -r) \
                     "$HOME/esp-idf" \
                     "$HOME/esp/esp-idf" \
                     "/opt/esp-idf"; do
        if [ -f "$candidate/tools/idf_tools.py" ]; then
            export IDF_PATH="$candidate"
            break
        fi
    done
fi

if [ -z "${IDF_PATH:-}" ] || [ ! -f "$IDF_PATH/tools/idf_tools.py" ]; then
    echo "Error: Cannot locate ESP-IDF in \$IDF_PATH or standard paths ($HOME/.espressif/v*/esp-idf)." >&2
    echo "Please set IDF_PATH to your ESP-IDF directory." >&2
    if [ "$_IS_SOURCED" -eq 1 ]; then
        return 1 2>/dev/null || exit 1
    else
        exit 1
    fi
fi

# Check whether cache generation is needed
_NEED_REGEN=0
if [ "${1:-}" == "--refresh" ] || [ "${1:-}" == "-f" ] || [ "$_IS_SOURCED" -eq 0 ]; then
    _NEED_REGEN=1
elif [ ! -s "$ENV_CACHE" ]; then
    _NEED_REGEN=1
else
    # Validate cached IDF_PATH matches current IDF_PATH
    _CACHED_IDF="$(grep -E "^(export )?IDF_PATH=" "$ENV_CACHE" 2>/dev/null | head -n1 | cut -d= -f2- | tr -d '"' || true)"
    if [ "$_CACHED_IDF" != "$IDF_PATH" ]; then
        _NEED_REGEN=1
    fi
fi

# Generate / refresh cached environment file
if [ "$_NEED_REGEN" -eq 1 ]; then
    _DETECTED_VER=""
    if [ -f "$IDF_PATH/version.txt" ]; then
        _DETECTED_VER="$(cat "$IDF_PATH/version.txt")"
    fi

    echo "Updating ESP-IDF environment cache ($IDF_PATH) -> .env.esp..."
    TMP_ENV="$(mktemp)"
    if python3 "$IDF_PATH/tools/idf_tools.py" export --format key-value > "$TMP_ENV" 2>/dev/null; then
        {
            echo "# Auto-generated ESP-IDF cached environment for Time Beacon"
            echo "IDF_PATH=\"$IDF_PATH\""
            [ -n "$_DETECTED_VER" ] && echo "ESP_IDF_VERSION=\"$_DETECTED_VER\""
            while IFS='=' read -r key val; do
                [ -z "$key" ] && continue
                # Replace absolute user home with $HOME for portable storage
                clean_val="$(echo "$val" | sed -e "s|^$HOME|\$HOME|g" -e "s|:$HOME|:\$HOME|g")"
                echo "$key=\"$clean_val\""
            done < "$TMP_ENV"
        } > "$ENV_CACHE"
        rm -f "$TMP_ENV"
    else
        rm -f "$TMP_ENV"
        if [ "$_IS_SOURCED" -eq 1 ] && [ -f "$IDF_PATH/export.sh" ]; then
            . "$IDF_PATH/export.sh"
            return 0 2>/dev/null || exit 0
        fi
    fi
fi

# If invoked directly (not sourced), we are done after regenerating cache
if [ "$_IS_SOURCED" -eq 0 ]; then
    echo "ESP-IDF environment cache refreshed successfully: $ENV_CACHE"
    exit 0
fi

# Sourced mode: Load environment into the active shell
if [ -f "$ENV_CACHE" ]; then
    while IFS= read -r line || [ -n "$line" ]; do
        # Ignore comments and empty lines
        [[ "$line" =~ ^[[:space:]]*# ]] && continue
        [[ -z "${line// }" ]] && continue
        # Evaluate export with $HOME expansion
        eval "export $line"
    done < "$ENV_CACHE"
fi

if command -v idf.py >/dev/null 2>&1; then
    echo "ESP-IDF activated: $(command -v idf.py)"
else
    if [ -f "$IDF_PATH/export.sh" ]; then
        . "$IDF_PATH/export.sh"
    fi
fi
