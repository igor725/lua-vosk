#!/bin/bash
if [ "$CC" = "" ]; then
	CC="gcc"
fi

$CC -pipe -fPIC -shared src/*.c -ldl -otest/vosk.so
