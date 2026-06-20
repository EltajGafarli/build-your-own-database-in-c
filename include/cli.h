#pragma once

#include<stdbool.h>
#include<stddef.h>

typedef enum {
    CLI_PARSE_OK,
    CLI_PARSE_INVALID
} CliParseResult;

typedef struct {
    const char *file_path;
    size_t bucket_count;
    bool bucket_count_provided;
    bool help_requested;
} CliConfig;

typedef enum {
    CLI_EXECUTE_OK,
    CLI_EXECUTE_EXIT_FAILURE,
    CLI_EXECUTE_EXIT_SUCCESS
} CliExecuteResult;

CliExecuteResult handle_cli_args(int argc, char *argv[], CliConfig *config);

void init_cli_config(CliConfig *config);
