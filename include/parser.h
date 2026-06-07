#pragma once

#define MAX_TOKEN_SIZE 256
#define MAX_INPUT_SIZE 1024

typedef enum {
    PARSE_SUCCESS,
    PARSE_EMPTY,
    PARSE_INVALID
} ParseResult;

typedef enum {
    CMD_HELP,
    CMD_EXIT,
    CMD_PUT,
    CMD_GET,
    CMD_DELETE,
    CMD_UNKNOWN
} CommandType;

typedef struct {
    CommandType type;
    char key[MAX_TOKEN_SIZE];
    char value[MAX_TOKEN_SIZE];
} ParsedCommand;

ParseResult parse_command(const char *input, ParsedCommand *command);
