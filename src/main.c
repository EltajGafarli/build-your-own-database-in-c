#include <stdio.h>
#include <stdbool.h>
#include "../include/command.h"
#include "../include/parser.h"
#include "../include/shell.h"
#include "../include/kv_store.h"
#include "../include/storage_log.h"

#define MAX_INPUT_SIZE 1024

int main(void) {
    char input[MAX_INPUT_SIZE];

    const char *file_path = "../data/pagedb.data";

    KvStore store;
    kv_store_init(&store);

    load_storage_log(&store, file_path);

    while (true) {
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

    return 0;
}