#!/bin/bash
MAINDIR=$(dirname `readlink -f "$0"`)
cd "$MAINDIR/luajit"
make clean
make HOST_CC=gcc CROSS=x86_64-linux-gnu- TARGET_SYS=Linux -j$(nproc)
cd "$MAINDIR"
mkdir -p out/linux-amd64
cd out/linux-amd64
cmake "$MAINDIR/.." -DCMAKE_TOOLCHAIN_FILE="$MAINDIR/TC-linux-x64.cmake" -DLUA_INCLUDE_DIR="$MAINDIR/luajit/src/" -DLUA_LIBRARIES="$MAINDIR/luajit/src/libluajit.so"
make -j$(nproc)
