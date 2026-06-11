#include "../include/storage_log.h"

#include <stdio.h>
#include <string.h>

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

