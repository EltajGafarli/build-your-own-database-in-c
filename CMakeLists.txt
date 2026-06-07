cmake_minimum_required(VERSION 3.20)
project(PageDB C)

set(CMAKE_C_STANDARD 17)

add_executable(PageDB src/main.c
        src/command.c
        src/shell.c
        src/parser.c
        src/kv_store.c)

target_include_directories(PageDB PRIVATE include)