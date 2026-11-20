#!/bin/bash
set -e

# Build script for Time Beacon targets
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Locate and load ESP-IDF environment
if [ -f "$SCRIPT_DIR/tools/idf_env.sh" ]; then
    . "$SCRIPT_DIR/tools/idf_env.sh"
elif command -v idf.py >/dev/null 2>&1; then
    echo "idf.py detected: $(command -v idf.py)"
elif [ -n "${IDF_PATH:-}" ] && [ -f "$IDF_PATH/export.sh" ]; then
    echo "Loading esp-idf from IDF_PATH ($IDF_PATH)..."
    . "$IDF_PATH/export.sh"
else
    echo "Warning: idf.py not found in PATH."
fi

# Auto-detect if project directory moved, invalidating CMake caches
check_stale_cache() {
    local bdir="$1"
    if [ -f "$bdir/CMakeCache.txt" ]; then
        local cache_home
        cache_home="$(grep "^CMAKE_HOME_DIRECTORY:INTERNAL=" "$bdir/CMakeCache.txt" 2>/dev/null | cut -d= -f2 || true)"
        if [ -n "$cache_home" ] && [ "$cache_home" != "$SCRIPT_DIR" ]; then
            echo "Detected relocated project (was: $cache_home, now: $SCRIPT_DIR). Purging $bdir..."
            rm -rf "$bdir"
        fi
    fi
}
check_stale_cache "build"
check_stale_cache "build_v1"
check_stale_cache "build_v2"

TARGET="${1:-all}"
EXTRA_ARG="${2:-}"
PORT="${3:-}"

# Auto-detect serial port if not provided
detect_port() {
    if [ -n "$PORT" ]; then
        echo "$PORT"
        return
    fi
    for p in /dev/ttyACM1 /dev/ttyACM0 /dev/ttyUSB0 /dev/ttyUSB1; do
        if [ -e "$p" ]; then
            echo "$p"
            return
        fi
    done
    echo "/dev/ttyACM1"
}

build_v1() {
    echo "=========================================="
    echo " Building Heltec Wireless Tracker v1.1..."
    echo "=========================================="
    idf.py -B build_v1 -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.v1_1" build
    # Also update default build/ so standard 'idf.py flash' uses v1.1
    mkdir -p build
    cp -r build_v1/* build/ 2>/dev/null || true
    echo ">> v1.1 build complete: build_v1/heltec-tracker-ntp.bin (synced to build/)"
}

build_v2() {
    echo "=========================================="
    echo " Building Heltec Wireless Tracker v2..."
    echo "=========================================="
    idf.py -B build_v2 -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.v2" build
    echo ">> v2 build complete: build_v2/heltec-tracker-ntp.bin"
}

run_tests() {
    echo "=========================================="
    echo " Running Native Host Test Suite...        "
    echo "=========================================="
    "$SCRIPT_DIR/tests/run_tests.sh"
}

flash_target() {
    local ver="${1:-v1.1}"
    local dev_port
    dev_port="$(detect_port)"
    echo "Flashing $ver to $dev_port..."
    if [ "$ver" = "v2" ]; then
        idf.py -B build_v2 -p "$dev_port" flash monitor
    else
        idf.py -B build_v1 -p "$dev_port" flash monitor
    fi
}

size_target() {
    local ver="${1:-v1.1}"
    local bdir="build_v1"
    if [ "$ver" = "v2" ]; then
        bdir="build_v2"
    fi
    if [ ! -d "$bdir" ]; then
        echo "Error: $bdir not found. Build target first via: $0 $ver"
        exit 1
    fi
    echo "=========================================="
    echo " Binary Footprint & Memory Audit ($ver)... "
    echo "=========================================="
    idf.py -B "$bdir" size
    if [ "${2:-}" = "components" ] || [ "${2:-}" = "-c" ]; then
        echo ""
        echo "Component Breakdown:"
        idf.py -B "$bdir" size-components
    fi
}

case "$TARGET" in
    test|tests)
        run_tests
        ;;
    size)
        size_target "$EXTRA_ARG" "$PORT"
        ;;
    v1|v1.1)
        build_v1
        ;;
    v2)
        build_v2
        ;;
    all)
        build_v1
        build_v2
        echo "=========================================="
        echo " Both targets built successfully!"
        echo "   v1.1: build_v1/heltec-tracker-ntp.bin"
        echo "   v2:   build_v2/heltec-tracker-ntp.bin"
        echo "=========================================="
        ;;
    flash)
        TARGET_VER="${EXTRA_ARG:-v1.1}"
        flash_target "$TARGET_VER"
        ;;
    clean)
        echo "Cleaning build directories..."
        rm -rf build build_v1 build_v2
        ;;
    *)
        echo "Usage: $0 [v1.1|v2|all|test|size [v1.1|v2] [components]|clean|flash [v1.1|v2] [port]]"
        exit 1
        ;;
esac
