@echo off
cl /MTd /Z7 /DDEBUG src\*.c /Fetest\vosk.dll /IC:\Users\igorg\Documents\GitHub\cserver\Lua\LuaJIT\src C:\Users\igorg\Documents\GitHub\cserver\Lua\LuaJIT\src\lua51.lib kernel32.lib /link /dll /noentry /def:src/luavosk.def
