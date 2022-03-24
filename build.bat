@echo off
set LUA_PATH=C:\Users\igorg\Documents\GitHub\cserver\Lua\LuaJIT\src
cl /MTd /Z7 /DDEBUG src\*.c /Fetest\vosk.dll /I%LUA_PATH% /link ^
/dll /noentry /def:src/luavosk.def /libpath:%LUA_PATH% lua51.lib kernel32.lib
