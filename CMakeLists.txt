cmake_minimum_required(VERSION 3.10)
project(CompiladorRossini C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O3")

include_directories(include)

set(SOURCES
    src/main.c
    src/lexer.c
    src/parser.c
    src/arena.c
    src/codegen.c
    src/symbol.c
)

add_executable(compiler ${SOURCES})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,noexecstack")
endif()