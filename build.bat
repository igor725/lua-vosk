@echo off
set LUA_PATH=D:\luajit\src
cl /MTd /Z7 /DDEBUG src\*.c /Fetest\vosk.dll /I%LUA_PATH% /link ^
/dll /noentry /def:src/luavosk.def /libpath:%LUA_PATH% lua51.lib kernel32.lib
