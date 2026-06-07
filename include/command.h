#pragma once

#include "parser.h"
#include "kv_store.h"

typedef enum {
    COMMAND_SUCCESS,
    COMMAND_EXIT,
    COMMAND_UNKNOWN
} CommandResult;

CommandResult handle_command(KvStore *kv_store, const ParsedCommand *command);

void handle_help(void);