#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include "../include/command.h"
#include "../include/parser.h"
#include "../include/shell.h"
#include "../include/kv_store.h"
#include "../include/storage_log.h"

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

    char input[MAX_INPUT_SIZE];

    const char *file_path = "../data/pagedb.data";

    if (argc == 2) {
        file_path = argv[1];
    } else if (argc > 2) {
        printf("Usage: PageDB [database_file]\n");
        return 1;
    }

    KvStore store;
    bool initialized = kv_store_init(&store);

    if (!initialized) {
        printf("Error: Failed to initialize KV store.\n");
        return EXIT_FAILURE;
    }

    load_storage_log(&store, file_path);

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

        CommandResult command_result = handle_command(&store, &command, file_path);

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