#!/usr/bin/env bash

set -euo pipefail

PLATFORM="mt8135"
QEMU="./build/qemu-system-arm"
BROM_IMAGE="bin/${PLATFORM}.bootrom.bin"
EMMC_IMAGE="bin/gpt.bin"
SDCARD_IMAGE="bin/preloader.sd"

for f in "$QEMU" "$BROM_IMAGE" "$EMMC_IMAGE" "$SDCARD_IMAGE"; do
  if [[ ! -f "$f" ]]; then
    echo "Error: missing file: $f" >&2
    exit 1
  fi
done

"$QEMU" \
  -M "$PLATFORM" \
  -d unimp \
  -bios "$BROM_IMAGE" \
  -drive file="$EMMC_IMAGE",format=raw,if=sd,index=0 \
  -drive file="$SDCARD_IMAGE",format=raw,if=sd,index=1 \
  -s \
  "$@" \
  2>&1