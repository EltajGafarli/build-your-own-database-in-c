#pragma once

#include "kv_store.h"
#include "parser.h"
#include <stdbool.h>

typedef enum READ_TOKEN_RESULT {
    SUCCESS, FAIL
} READ_TOKEN_RESULT;

void load_storage_log(KvStore *store, const char *filename);

bool storage_log_append(ParsedCommand *command, const char *filename);