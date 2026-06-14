#include "../include/parser.h"

#include <string.h>
#include <stdio.h>

static ParseResult validate_command(const ParsedCommand *command, int token_count);
static void init_parsed_command(ParsedCommand *command);
static CommandType parse_command_type(const char *token);
static void copy_token(char *destination, const char *source);

ParseResult parse_command(const char *input, ParsedCommand *command) {

    init_parsed_command(command);

    char input_copy[MAX_INPUT_SIZE];

    copy_token(input_copy, input);

    char *token = strtok(input_copy, " ");

    if (token == NULL || strlen(token) == 0) {
        return PARSE_EMPTY;
    }

    int token_count = 0;

    while (token != NULL) {

        if (token_count == 0) {
            command->type = parse_command_type(token);
        } else if (token_count == 1) {
            strncpy(command->key, token, MAX_TOKEN_SIZE - 1);
            command->key[MAX_TOKEN_SIZE - 1] = '\0';
        } else if (token_count == 2) {
            strncpy(command->value, token, MAX_TOKEN_SIZE - 1);
            command->value[MAX_TOKEN_SIZE - 1] = '\0';
        } else if (token_count > 2 && command->type != CMD_PUT) {
            command->type = CMD_UNKNOWN;
            break;
        }

        if (token_count >= 3) {

            size_t new_val_len = strlen(command->value) + strlen(token) + 1;

            if (new_val_len < MAX_TOKEN_SIZE) {
                strcat(command->value, " ");
                strcat(command->value, token);
            } else {
                command->type = CMD_UNKNOWN;
                break;
            }
        }

        token = strtok(NULL, " ");

        token_count ++;
    }

    return validate_command(command, token_count);
}

static ParseResult validate_command(const ParsedCommand *command, int token_count) {
    switch (command->type) {
        case CMD_HELP:
        case CMD_EXIT:
        case CMD_COMPACT:
        case CMD_STATS:
            return token_count == 1 ? PARSE_SUCCESS : PARSE_INVALID;

        case CMD_PUT:
            return token_count >= 3 ? PARSE_SUCCESS : PARSE_INVALID;

        case CMD_GET:
        case CMD_DELETE:
            return token_count == 2 ? PARSE_SUCCESS : PARSE_INVALID;

        case CMD_UNKNOWN:
        default:
            return PARSE_INVALID;
    }
}

static void init_parsed_command(ParsedCommand *command) {
    command->type = CMD_UNKNOWN;
    command->key[0] = '\0';
    command->value[0] = '\0';
}

static CommandType parse_command_type(const char *token) {
    if (strcmp(token, "help") == 0) {
        return CMD_HELP;
    }

    if (strcmp(token, "exit") == 0 || strcmp(token, "quit") == 0) {
        return CMD_EXIT;
    }

    if (strcmp(token, "put") == 0) {
        return CMD_PUT;
    }

    if (strcmp(token, "get") == 0) {
        return CMD_GET;
    }

    if (strcmp(token, "delete") == 0) {
        return CMD_DELETE;
    }

    if (strcmp(token, "compact") == 0) {
        return CMD_COMPACT;
    }

    if (strcmp(token, "stats") == 0) {
        return CMD_STATS;
    }

    return CMD_UNKNOWN;
}

static void copy_token(char *destination, const char *source) {
    strncpy(destination, source, MAX_INPUT_SIZE - 1);
    destination[MAX_INPUT_SIZE - 1] = '\0';
}
