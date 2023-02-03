@echo off
setlocal
if "%LUA_PATH%"=="" (
	set LUA_PATH=D:\luajit\src
)
mkdir objs 2>nul
cl /MTd /Z7 /DDEBUG src\*.c /Foobjs\ /Fetest\vosk.dll /I%LUA_PATH% /link ^
/dll /noentry /def:src/luavosk.def /libpath:%LUA_PATH% lua51.lib kernel32.lib
endlocal
