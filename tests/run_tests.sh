#!/bin/bash
set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$DIR")"

echo "============================================"
echo " Building and Running Native Host Tests...  "
echo "============================================"

gcc -Wall -Wextra -std=gnu17 -O2 \
    -I "${PROJECT_ROOT}/main/include" \
    "${DIR}/test_harness.c" \
    -o "${DIR}/test_runner"

"${DIR}/test_runner"

rm -f "${DIR}/test_runner"
echo "All tests clean!"
