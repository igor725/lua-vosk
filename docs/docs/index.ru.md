# Введение

Вы можете найти библиотеку [lua-vosk](https://github.com/igor725/lua-vosk) на [моей странице GitHub](https://github.com/igor725).

## Зависимости
1. [LuaJIT](https://luajit.org/)/[Lua](https://lua.org/) >= 5.1
2. [Библиотека Vosk](https://github.com/alphacep/vosk-api/releases/latest)

## Сборка

Для сборки понадобится [CMake](https://cmake.org/):

- На Windows:
```batch
mkdir build
cd build
cmake ..
msbuild luavosk.sln
```

- На Linux/WSL:
```bash
mkdir build
cd build
cmake ..
make -j4
```
