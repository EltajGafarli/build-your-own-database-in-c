#include "command.h"
#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "kv_store.h"
#include "storage_log.h"

void handle_help(void) {
    printf("\nAvailable Commands:\n");
    printf("  help   - Display this menu\n");
    printf("  put    - Store key-value pair: put <key> <value>\n");
    printf("  get    - Get value by key: get <key>\n");
    printf("  delete - Delete value by key: delete <key>\n");
    printf("  exit   - Close this application\n");
    printf("  quit   - Close this application\n\n");
    printf("  compact - Rewrite log with active records only\n");
    printf("  stats   - Show database statistics\n");
}

CommandResult handle_command(KvStore *store, ParsedCommand *command, const char *file_path) {

    if (command->type == CMD_PUT || command->type == CMD_DELETE) {

        if (command->type == CMD_DELETE) {
            const char *value = NULL;

            KvResult result = kv_store_get(store, command->key, &value);

            if (result == KV_NOT_FOUND) {
                printf("NOT FOUND\n");
                return COMMAND_SUCCESS;
            }
        }

        bool is_append = storage_log_append(command, file_path);

        if (!is_append) {
            printf("Error: could not append to log file\n");
            return COMMAND_ERROR;
        }
    }

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

        case CMD_COMPACT: {
            bool compact_success = storage_log_compact(store, file_path);

            if (!compact_success) {
                return COMMAND_ERROR;
            }
            printf("OK\n");
            return COMMAND_SUCCESS;
        }

        case CMD_STATS: {
            printf("Active records: %zu\nMax records: %zu\nStorage file: %s\n",
                store->size,
                (size_t)KV_MAX_ITEMS,
                file_path);

            long file_size = storage_log_get_file_size(file_path);

            if (file_size >= 0) {
                printf("Storage file size: %ld bytes\n", file_size);
            } else {
                printf("Storage file size: 0 bytes\n");
            }

            double load_factor = (double) store->size / (double) DEFAULT_BUCKET_SIZE;

            printf("Bucket count: %zu\n", (size_t) DEFAULT_BUCKET_SIZE);
            printf("Load factor: %.2f\n", load_factor);

            return COMMAND_SUCCESS;
        }

        case CMD_UNKNOWN:
        default:
            return COMMAND_UNKNOWN;
    }
}