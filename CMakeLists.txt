cmake_minimum_required(VERSION 3.20)
project(PageDB C)

set(CMAKE_C_STANDARD 17)

add_executable(PageDB src/main.c
        src/command.c
        src/shell.c
        src/parser.c
        src/kv_store.c
        src/storage_log.c
        src/cli.c)

target_include_directories(PageDB PRIVATE include)

enable_testing()

add_executable(test_cli
        ./tests/cli/test_cli.c
        src/cli.c
)

target_include_directories(test_cli PRIVATE include)

add_test(NAME cli_tests COMMAND test_cli)