@echo off
setlocal
if "%LUA_PATH%"=="" (
	set LUA_PATH=D:\luajit\src
)

if "%LUA_LIB_PATH%"=="" set LUA_LIB_PATH=%LUA_PATH%
if "%LUA_INCLUDE_PATH%"=="" set LUA_INCLUDE_PATH=%LUA_PATH%
set VOSK_DBGFLAGS=/Zi /DDEBUG /D_DEBUG
set VOSK_DEFINES=/DVOSK_ENABLE_JSON
set VOSK_EXEC=test\vosk.dll

mkdir objs 2>nul
cl /MD %VOSK_DBGFLAGS% %VOSK_DEFINES% src\*.c /Foobjs\ /Fe%VOSK_EXEC% /I%LUA_INCLUDE_PATH% /link ^
/dll /debug /libpath:%LUA_LIB_PATH% lua51.lib /EXPORT:luaopen_vosk
endlocal
