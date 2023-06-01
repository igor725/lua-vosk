cmake_minimum_required(VERSION 3.12)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
  message(FATAL_ERROR "Prevented in-tree build. Please create a build directory outside of the game source code and run \"cmake -S ${CMAKE_SOURCE_DIR} -B .\" from there")
endif()

# Включает/выключает преобразование JSON строки в Lua таблицу
set(USE_JSON_PARSER yes)

project(luavosk LANGUAGES C VERSION "1.0.0")

add_library(luavosk SHARED
	src/main.c
	src/luamodel.c
	src/luarecognizer.c
	src/luaspkmodel.c
	src/voskbridge.c
)

find_package(Lua REQUIRED)
if(USE_JSON_PARSER STREQUAL yes)
	target_compile_definitions(luavosk PRIVATE VOSK_ENABLE_JSON)
endif()
target_link_libraries(luavosk ${LUA_LIBRARIES})
target_include_directories(luavosk PRIVATE ${LUA_INCLUDE_DIR})
set_target_properties(luavosk PROPERTIES PREFIX "")
