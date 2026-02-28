cmake_minimum_required(VERSION 3.10)
project(CompiladorRossini C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O3")

# 1. Avisa ao CMake onde procurar os headers (.h)
include_directories(include)

# 2. Lista os arquivos que agora estão dentro de src/
set(SOURCES
    src/main.c
    src/lexer.c
    src/parser.c
    src/arena.c
    src/codegen.c
)

add_executable(compiler ${SOURCES})

# Dica de Ouro: Adicione isso para remover aquele aviso de segurança da Pilha!
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,noexecstack")
endif()