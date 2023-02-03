#!/bin/bash
if [ "$CC" = "" ]; then
	CC="gcc"
fi

if pkg-config --exists luajit; then
	LUA_FLAGS=$(pkg-config --libs --cflags luajit)
elif pkg-config --exists lua; then
	LUA_FLAGS=$(pkg-config --libs --cflags lua)
else
	echo "Lua not found!"
	exit 1
fi

$CC -rdynamic -pipe -fPIC -shared src/*.c -Isrc $LUA_FLAGS -ldl -otest/vosk.so
echo "Output binary: ./test/vosk.so"
