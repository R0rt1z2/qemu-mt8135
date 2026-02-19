#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="build"
JOBS="$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)"

usage() {
    echo "Usage: $(basename "$0") [-f] [-j JOBS] [-h]"
    echo "  -f        Force clean rebuild"
    echo "  -j JOBS   Number of parallel jobs (default: $JOBS)"
    echo "  -h        Show this help"
    exit "${1:-0}"
}

while getopts "fj:h" opt; do
    case "$opt" in
        f) FORCE=1 ;;
        j) JOBS="$OPTARG" ;;
        h) usage ;;
        *) usage 1 ;;
    esac
done

if [[ "${FORCE:-0}" -eq 1 ]]; then
    echo "Forcing clean rebuild..."
    rm -rf "$BUILD_DIR"
fi

if [[ ! -d "$BUILD_DIR" ]]; then
    echo "Configuring..."
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    ../configure
else
    echo "Build directory exists, skipping configure."
    cd "$BUILD_DIR"
fi

echo "Building with $JOBS jobs..."
make -j"$JOBS"