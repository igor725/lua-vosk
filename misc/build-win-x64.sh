#!/bin/bash
mkdir -p out/win-x64
cd out/win-x64
cmake -DCMAKE_TOOLCHAIN_FILE=./TC-mingw.cmake ../../
