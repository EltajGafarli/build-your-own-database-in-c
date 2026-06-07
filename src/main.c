#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../include/command.h"
#include "../include/parser.h"
#include "../include/shell.h"

#define MAX_INPUT_SIZE 1024

int main(void) {
    char input[MAX_INPUT_SIZE];

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

        CommandResult command_result = handle_command(&command);

        if (command_result == COMMAND_EXIT) {
            break;
        } else if (command_result == COMMAND_SUCCESS) {
            continue;
        } else if (command_result == COMMAND_UNKNOWN) {
            printf("Error: Command '%s' not recognized. Type 'help'.\n", input);
        } else {
            printf("Error: Unknown error occurred.\n");
        }
    }

    return 0;
}