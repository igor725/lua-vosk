#!/bin/bash
MAINDIR=$(dirname `readlink -f "$0"`)
rm -rf "$(MAINDIR)/out/"
git clone https://github.com/LuaJIT/LuaJIT luajit
