#pragma once

#include "parser.h"
#include "kv_store.h"

typedef enum {
    COMMAND_SUCCESS,
    COMMAND_EXIT,
    COMMAND_UNKNOWN,
    COMMAND_ERROR
} CommandResult;

CommandResult handle_command(KvStore *kv_store, ParsedCommand *command, const char *file_path);

void handle_help(void);