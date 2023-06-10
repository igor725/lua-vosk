# Getting started

You can find the [lua-vosk](https://github.com/igor725/lua-vosk) library on [my GitHub page](https://github.com/igor725).

## Dependencies
1. [LuaJIT](https://luajit.org/)/[Lua](https://lua.org/) >= 5.1
2. [Vosk library](https://github.com/alphacep/vosk-api/releases/latest)

## Building

Use [CMake](https://cmake.org/) to build the project:

- On Windows:
```batch
mkdir build
cd build
cmake ..
msbuild luavosk.sln
```

- On Linux/WSL:
```bash
mkdir build
cd build
cmake ..
make -j4
```
