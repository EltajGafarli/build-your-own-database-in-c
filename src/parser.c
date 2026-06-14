#include "../include/parser.h"

#include <string.h>
#include <stddef.h>

static ParseResult validate_command(const ParsedCommand *command, int token_count);
static void init_parsed_command(ParsedCommand *command);
static CommandType parse_command_type(const char *token);

static const char *skip_spaces(const char *p);
static void trim_right(char *str);
static int copy_range(char *destination, size_t destination_size, const char *start, size_t length);
static int copy_string(char *destination, size_t destination_size, const char *source);

ParseResult parse_command(const char *input, ParsedCommand *command) {
    init_parsed_command(command);

    char input_copy[MAX_INPUT_SIZE];

    if (!copy_string(input_copy, sizeof(input_copy), input)) {
        return PARSE_INVALID;
    }

    trim_right(input_copy);

    const char *p = skip_spaces(input_copy);

    if (*p == '\0') {
        return PARSE_EMPTY;
    }

    const char *command_start = p;

    while (*p != '\0' && *p != ' ' && *p != '\t') {
        p++;
    }

    char command_token[MAX_TOKEN_SIZE];

    if (!copy_range(command_token, sizeof(command_token), command_start, (size_t)(p - command_start))) {
        return PARSE_INVALID;
    }

    command->type = parse_command_type(command_token);

    p = skip_spaces(p);

    if (command->type == CMD_PUT) {
        if (*p == '\0') {
            return PARSE_INVALID;
        }

        const char *key_start = p;

        while (*p != '\0' && *p != ' ' && *p != '\t') {
            p++;
        }

        if (!copy_range(command->key, sizeof(command->key), key_start, (size_t)(p - key_start))) {
            return PARSE_INVALID;
        }

        p = skip_spaces(p);

        if (*p == '\0') {
            return PARSE_INVALID;
        }

        if (!copy_string(command->value, sizeof(command->value), p)) {
            return PARSE_INVALID;
        }

        return validate_command(command, 3);
    }

    int token_count = 1;

    while (*p != '\0') {
        const char *token_start = p;

        while (*p != '\0' && *p != ' ' && *p != '\t') {
            p++;
        }

        token_count++;

        if (token_count == 2) {
            if (!copy_range(command->key, sizeof(command->key), token_start, (size_t)(p - token_start))) {
                return PARSE_INVALID;
            }
        }

        p = skip_spaces(p);
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
            return token_count == 3 &&
                   command->key[0] != '\0' &&
                   command->value[0] != '\0'
                   ? PARSE_SUCCESS
                   : PARSE_INVALID;

        case CMD_GET:
        case CMD_DELETE:
            return token_count == 2 &&
                   command->key[0] != '\0'
                   ? PARSE_SUCCESS
                   : PARSE_INVALID;

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

static const char *skip_spaces(const char *p) {
    while (*p == ' ' || *p == '\t') {
        p++;
    }

    return p;
}

static void trim_right(char *str) {
    size_t len = strlen(str);

    while (len > 0 &&
           (str[len - 1] == '\n' ||
            str[len - 1] == '\r' ||
            str[len - 1] == ' ' ||
            str[len - 1] == '\t')) {
        str[len - 1] = '\0';
        len--;
    }
}

static int copy_range(char *destination, size_t destination_size, const char *start, size_t length) {
    if (length >= destination_size) {
        return 0;
    }

    memcpy(destination, start, length);
    destination[length] = '\0';

    return 1;
}

static int copy_string(char *destination, size_t destination_size, const char *source) {
    size_t source_len = strlen(source);

    if (source_len >= destination_size) {
        return 0;
    }

    memcpy(destination, source, source_len + 1);

    return 1;
}