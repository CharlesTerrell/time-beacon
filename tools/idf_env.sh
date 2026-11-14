#!/usr/bin/env bash
# Source this file to activate the ESP-IDF environment:
#   source tools/idf_env.sh

# 1. Fast path: already active in current shell
if command -v idf.py >/dev/null 2>&1; then
    return 0 2>/dev/null || exit 0
fi

# Locate workspace root directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ENV_CACHE="$SCRIPT_DIR/.env.esp"

# 2. Discover local ESP-IDF installation if IDF_PATH not already set
if [ -z "${IDF_PATH:-}" ] || [ ! -f "$IDF_PATH/tools/idf_tools.py" ]; then
    # Look for highest vX.Y in ~/.espressif or standard ~/esp/esp-idf locations
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
    echo "Error: ESP-IDF not found in \$IDF_PATH or standard paths ($HOME/.espressif/v*/esp-idf)." >&2
    echo "Please set IDF_PATH to your ESP-IDF directory." >&2
    return 1 2>/dev/null || exit 1
fi

# Detect installed ESP-IDF version
_DETECTED_VER=""
if [ -f "$IDF_PATH/version.txt" ]; then
    _DETECTED_VER="$(cat "$IDF_PATH/version.txt")"
fi

# 3. Check cached environment
_NEED_REGEN=0
if [ ! -s "$ENV_CACHE" ]; then
    _NEED_REGEN=1
else
    # Verify cached IDF_PATH matches current IDF_PATH
    _CACHED_IDF="$(grep "^export IDF_PATH=" "$ENV_CACHE" 2>/dev/null | head -n1 | cut -d= -f2- | tr -d '"' || true)"
    if [ "$_CACHED_IDF" != "$IDF_PATH" ]; then
        _NEED_REGEN=1
    fi
fi

# 4. Generate or update cached environment if missing or stale
if [ "$_NEED_REGEN" -eq 1 ]; then
    echo "Caching ESP-IDF environment ($IDF_PATH) -> .env.esp..."
    # Generate portable key-value exports using python
    TMP_ENV="$(mktemp)"
    if python3 "$IDF_PATH/tools/idf_tools.py" export --format key-value > "$TMP_ENV" 2>/dev/null; then
        {
            echo "# Auto-generated ESP-IDF cached environment"
            echo "export IDF_PATH=\"$IDF_PATH\""
            [ -n "$_DETECTED_VER" ] && echo "export ESP_IDF_VERSION=\"$_DETECTED_VER\""
            # Format exports with portable $HOME substitution
            while IFS='=' read -r key val; do
                [ -z "$key" ] && continue
                # Replace absolute home with $HOME for upload/portability safety
                clean_val="$(echo "$val" | sed -e "s|^$HOME|\$HOME|g" -e "s|:$HOME|:\$HOME|g")"
                echo "export $key=\"$clean_val\""
            done < "$TMP_ENV"
        } > "$ENV_CACHE"
        rm -f "$TMP_ENV"
    else
        rm -f "$TMP_ENV"
        # Fallback to export.sh if idf_tools.py export fails
        if [ -f "$IDF_PATH/export.sh" ]; then
            . "$IDF_PATH/export.sh"
            return 0 2>/dev/null || exit 0
        fi
    fi
fi

# 5. Load cached environment instantaneously
if [ -f "$ENV_CACHE" ]; then
    # Expand $HOME when sourcing
    eval "$(< "$ENV_CACHE")"
fi

if command -v idf.py >/dev/null 2>&1; then
    echo "ESP-IDF activated: $(command -v idf.py)"
else
    # Fallback to export.sh
    if [ -f "$IDF_PATH/export.sh" ]; then
        . "$IDF_PATH/export.sh"
    fi
fi
