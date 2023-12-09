#!/bin/bash
set -e
MAINDIR=$(dirname `readlink -f "$0"`)
if [ "$1" = "luajit" ]; then
	git clone https://github.com/LuaJIT/LuaJIT "$MAINDIR/luajit"
else
	curl -O https://www.lua.org/ftp/lua-5.1.5.tar.gz
	tar xvf lua-5.1.5.tar.gz
	rm lua-5.1.5.tar.gz
fi
