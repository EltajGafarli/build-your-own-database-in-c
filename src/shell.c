#include "../include/shell.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool shell_read_input(char *buffer, size_t buffer_size) {
    printf("pagedb> ");
    fflush(stdout);

    if (fgets(buffer, buffer_size, stdin) == NULL) {
        return false;
    }

    buffer[strcspn(buffer, "\n")] = '\0';

    return true;
}