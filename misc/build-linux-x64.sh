#!/bin/bash
mkdir -p out/linux-x64
cd out/linux-x64
cmake -DCMAKE_TOOLCHAIN_FILE=./TC-linux.cmake ../../
