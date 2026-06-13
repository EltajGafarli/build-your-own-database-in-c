#include "../include/storage_log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void load_storage_log(KvStore *store, const char *filename) {
    FILE * fp;

    int bufferLength = 255;
    char buffer[bufferLength];

    fp = fopen(filename, "r");

    if (fp == NULL) {
        return;
    }

    while(fgets(buffer, bufferLength, fp)) {

        buffer[strcspn(buffer, "\n")] = '\0';;

        ParsedCommand command;

        ParseResult parse_result = parse_command(buffer, &command);

        if (parse_result == PARSE_SUCCESS) {

            if (command.type == CMD_PUT) {
                kv_store_put(store, command.key, command.value);
            } else if (command.type == CMD_DELETE) {
                kv_store_delete(store, command.key);
            }
        }
    }

    fclose(fp);

}

bool storage_log_append(ParsedCommand *command, const char *filename) {
    if (command->type == CMD_PUT) {

        FILE * fp = fopen(filename, "a");

        if (fp != NULL) {
            fprintf(fp, "%s %s %s\n", "put", command->key, command->value);
            fclose(fp);
            return true;
        }

        return false;
    } else if (command->type == CMD_DELETE) {
        FILE * fp = fopen(filename, "a");

        if (fp != NULL) {
            fprintf(fp, "%s %s\n", "delete", command->key);
            fclose(fp);
            return true;
        }
        return false;
    } else {
        return true;
    }
}

bool storage_log_compact(KvStore *store, const char *filename) {

    const char *suffix = ".tmp";

    size_t temp_len = strlen(filename) + strlen(suffix) + 1;

    char *temp_filename = malloc(temp_len);

    if (temp_filename == NULL) {
        return false;
    }

    snprintf(temp_filename, temp_len, "%s%s", filename, suffix);

    FILE * fp = fopen(temp_filename, "w");

    if (fp == NULL) {
        free(temp_filename);
        return false;
    }

    for (size_t i = 0; i < DEFAULT_BUCKET_SIZE; i++) {

        KvEntry *head = store->buckets[i];

        while (head != NULL) {
            fprintf(fp, "%s %s %s\n", "put", head->key, head->value);
            head = head->next;
        }
    }

    if (fclose(fp) != 0) {
        remove(temp_filename);
        free(temp_filename);
        return false;
    }

    remove(filename);

    if (rename(temp_filename, filename) != 0) {
        remove(temp_filename);
        free(temp_filename);
        return false;
    }

    free(temp_filename);
    return true;
}
