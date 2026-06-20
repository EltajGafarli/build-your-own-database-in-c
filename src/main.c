#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include "../include/command.h"
#include "../include/parser.h"
#include "../include/shell.h"
#include "../include/kv_store.h"
#include "../include/storage_log.h"
#include "../include/cli.h"

#define MAX_INPUT_SIZE 1024

static volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
    keep_running = 0;
}

int main(int argc, char *argv[]) {

    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("Error registering signal handler");
        return EXIT_FAILURE;
    }

    CliConfig cli_config;

    init_cli_config(&cli_config);

    CliExecuteResult cli_result = handle_cli_args(argc, argv, &cli_config);

    if (cli_result == CLI_EXECUTE_EXIT_FAILURE) {
        return EXIT_FAILURE;
    } else if (cli_result == CLI_EXECUTE_EXIT_SUCCESS) {
        return EXIT_SUCCESS;
    }

    KvStore store;
    bool initialized;

    if (cli_config.bucket_count_provided) {
        initialized = kv_store_init_with_bucket_count(&store, cli_config.bucket_count);
    } else {
        initialized = kv_store_init(&store);
    }

    if (!initialized) {
        printf("Error: Failed to initialize KV store.\n");
        return EXIT_FAILURE;
    }

    const char *file_path = cli_config.file_path;

    load_storage_log(&store, file_path);

    char input[MAX_INPUT_SIZE];

    while (keep_running) {

        bool success = shell_read_input(input, sizeof(input));

        if (!success) {
            printf("\nExiting due to EOF.\n");
            break;
        }

        ParsedCommand command;
        ParseResult parse_result = parse_command(input, &command);

        if (parse_result == PARSE_EMPTY) {
            continue;
        }

        if (parse_result == PARSE_INVALID) {
            printf("Error: invalid command usage\n");
            continue;
        }

        const CommandResult command_result = handle_command(&store, &command, file_path);

        if (command_result == COMMAND_EXIT) {
            break;
        } else if (command_result == COMMAND_SUCCESS) {
            continue;
        } else if (command_result == COMMAND_UNKNOWN) {
            printf("Error: Command '%s' not recognized. Type 'help'.\n", input);
        } else if (command_result == COMMAND_ERROR) {
            continue;
        } else {
            printf("Error: Unknown error occurred.\n");
        }
    }

    kv_store_destroy(&store);

    return EXIT_SUCCESS;
}
