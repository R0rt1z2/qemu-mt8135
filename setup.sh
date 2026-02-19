#!/usr/bin/env bash

if command -v apt &>/dev/null; then
    sudo apt update
    sudo apt install -y ninja-build libglib2.0-dev libpixman-1-dev \
        pkg-config python3-venv flex bison gcc make git \
        libslirp-dev libaio-dev libseccomp-dev libcap-ng-dev libzstd-dev
elif command -v pacman &>/dev/null; then
    sudo pacman -Syu --noconfirm ninja glib2 pixman \
        pkg-config python flex bison base-devel git \
        libslirp libaio libseccomp libcap-ng zstd
else
    echo "Unsupported package manager. Install dependencies manually."
    exit 1
fi
