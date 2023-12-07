#!/bin/bash
mkdir -p out/win-arm64
cd out/win-arm64
cmake -DCMAKE_TOOLCHAIN_FILE=./TC-arm64.cmake ../../
