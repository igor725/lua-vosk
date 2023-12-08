#!/bin/bash
MAINDIR=$(dirname `readlink -f "$0"`)
cd "$MAINDIR/luajit"
make clean
make HOST_CC=gcc CROSS=x86_64-w64-mingw32- TARGET_SYS=Windows -j$(nproc)
cd "$MAINDIR"
mkdir -p out/win-amd64
cd out/win-amd64
cmake "$MAINDIR/.." -DCMAKE_TOOLCHAIN_FILE="$MAINDIR/TC-win-x64.cmake" -DLUA_INCLUDE_DIR="$MAINDIR/luajit/src/" -DLUA_LIBRARIES="$MAINDIR/luajit/src/lua51.dll"
make -j$(nproc)
