#include "command.h"
#include <stdio.h>
#include <string.h>
#include "parser.h"

void handle_help(void) {
    printf("\nAvailable Commands:\n");
    printf("  help   - Display this menu\n");
    printf("  put    - Store key-value pair: put <key> <value>\n");
    printf("  get    - Get value by key: get <key>\n");
    printf("  delete - Delete value by key: delete <key>\n");
    printf("  exit   - Close this application\n");
    printf("  quit   - Close this application\n\n");
}

CommandResult handle_command(const ParsedCommand *command) {
    switch (command->type) {
        case CMD_PUT:
            printf("PUT command detected\nkey: %s\nvalue: %s\n", command->key, command->value);
            return COMMAND_SUCCESS;

        case CMD_GET:
            printf("GET command detected\nkey: %s\n", command->key);
            return COMMAND_SUCCESS;

        case CMD_DELETE:
            printf("DELETE command detected\nkey: %s\n", command->key);
            return COMMAND_SUCCESS;

        case CMD_HELP:
            handle_help();
            return COMMAND_SUCCESS;

        case CMD_EXIT:
            printf("Goodbye!\n");
            return COMMAND_EXIT;

        case CMD_UNKNOWN:
        default:
            return COMMAND_UNKNOWN;
    }
}