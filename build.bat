@echo off
cl src\*.c /Fevosk.dll /IC:\Users\igorg\Documents\GitHub\cserver\Lua\LuaJIT\src C:\Users\igorg\Documents\GitHub\cserver\Lua\LuaJIT\src\lua51.lib kernel32.lib /link /dll /noentry
