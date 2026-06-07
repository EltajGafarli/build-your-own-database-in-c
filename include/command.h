#pragma once

#include "parser.h"

typedef enum {
    COMMAND_SUCCESS,
    COMMAND_EXIT,
    COMMAND_UNKNOWN
} CommandResult;

CommandResult handle_command(const ParsedCommand *command);

void handle_help(void);