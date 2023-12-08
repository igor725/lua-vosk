#!/bin/bash
set -e
MAINDIR=$(dirname `readlink -f "$0"`)
git clone https://github.com/LuaJIT/LuaJIT "$MAINDIR/luajit"
