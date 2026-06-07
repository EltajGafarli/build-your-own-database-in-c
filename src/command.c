#include "command.h"
#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "kv_store.h"

void handle_help(void) {
    printf("\nAvailable Commands:\n");
    printf("  help   - Display this menu\n");
    printf("  put    - Store key-value pair: put <key> <value>\n");
    printf("  get    - Get value by key: get <key>\n");
    printf("  delete - Delete value by key: delete <key>\n");
    printf("  exit   - Close this application\n");
    printf("  quit   - Close this application\n\n");
}

CommandResult handle_command(KvStore *store, const ParsedCommand *command) {
    switch (command->type) {
        case CMD_PUT: {
            KvResult result = kv_store_put(store, command->key, command->value);

            if (result == KV_OK) {
                printf("OK\n");
            } else {
                printf("ERROR\n");
            }

            return COMMAND_SUCCESS;
        }

        case CMD_GET: {
            const char *value = NULL;

            KvResult result = kv_store_get(store, command->key, &value);

            if (result == KV_OK) {
                printf("%s\n", value);
            } else {
                printf("NOT FOUND\n");
            }
            return COMMAND_SUCCESS;
        }

        case CMD_DELETE: {

            KvResult result = kv_store_delete(store, command->key);

            if (result == KV_OK) {
                printf("OK\n");
            } else {
                printf("NOT FOUND\n");
            }
            return COMMAND_SUCCESS;
        }

        case CMD_HELP: {
            handle_help();
            return COMMAND_SUCCESS;
        }

        case CMD_EXIT: {
            printf("Goodbye!\n");
            return COMMAND_EXIT;
        }

        case CMD_UNKNOWN:
        default:
            return COMMAND_UNKNOWN;
    }
}