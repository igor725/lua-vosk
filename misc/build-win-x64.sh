#!/bin/bash
set -e
MAINDIR=$(dirname `readlink -f "$0"`)
if [ "$1" = "luajit" ]; then
	LUAPATH="$MAINDIR/luajit"
	cd "$LUAPATH"
	make clean
	make HOST_CC=gcc CROSS=x86_64-w64-mingw32- TARGET_SYS=Windows -j$(nproc)
else
	LUAPATH="$MAINDIR/lua-5.1.5"
	cd "$LUAPATH"
	make clean
	make CC=x86_64-w64-mingw32-gcc "AR=x86_64-w64-mingw32-ar rcu" PLAT=mingw -j$(nproc)
fi

cd "$MAINDIR"
mkdir -p out/win-amd64
cd out/win-amd64
cmake "$MAINDIR/.." -DCMAKE_TOOLCHAIN_FILE="$MAINDIR/TC-win-x64.cmake" -DLUA_INCLUDE_DIR="$LUAPATH/src/" -DLUA_LIBRARIES="$LUAPATH/src/lua51.dll"
make -j$(nproc)
