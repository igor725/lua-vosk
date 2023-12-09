#!/bin/bash
set -e
MAINDIR=$(dirname `readlink -f "$0"`)
if [ "$1" = "luajit" ]; then
	LUAPATH="$MAINDIR/luajit"
	LUALIB="libluajit.so"
	cd "$LUAPATH"
	make clean
	make HOST_CC=gcc CROSS=x86_64-linux-gnu- TARGET_SYS=Linux -j$(nproc)
else
	LUAPATH="$MAINDIR/lua-5.1.5"
	LUALIB="liblua.a"
	cd "$LUAPATH"
	make clean
	make "CC=x86_64-linux-gnu-gcc -fPIC" "AR=x86_64-linux-gnu-gcc-ar rcu" PLAT=generic -j$(nproc)
fi

cd "$MAINDIR"
mkdir -p out/linux-amd64
cd out/linux-amd64
cmake "$MAINDIR/.." -DCMAKE_TOOLCHAIN_FILE="$MAINDIR/TC-linux-x64.cmake" -DLUA_INCLUDE_DIR="$LUAPATH/src/" -DLUA_LIBRARIES="$LUAPATH/src/$LUALIB"
make -j$(nproc)
