@echo off
setlocal
if "%LUA_PATH%"=="" (
	set LUA_PATH=D:\luajit\src
)

if "%LUA_LIB_PATH%"=="" set LUA_LIB_PATH=%LUA_PATH%
if "%LUA_INCLUDE_PATH%"=="" set LUA_INCLUDE_PATH=%LUA_PATH%

mkdir objs 2>nul
cl /MTd /Z7 /DDEBUG src\*.c /Foobjs\ /Fetest\vosk.dll /I%LUA_INCLUDE_PATH% /link ^
/dll /noentry /libpath:%LUA_LIB_PATH% lua51.lib kernel32.lib /EXPORT:luaopen_vosk
endlocal
