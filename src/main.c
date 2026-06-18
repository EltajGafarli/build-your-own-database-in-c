#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include "../include/command.h"
#include "../include/parser.h"
#include "../include/shell.h"
#include "../include/kv_store.h"
#include "../include/storage_log.h"

#define MAX_INPUT_SIZE 1024

typedef enum {
    CLI_PARSE_OK,
    CLI_PARSE_INVALID
} CliParseResult;

typedef struct {
    const char *file_path;
    size_t bucket_count;
    bool bucket_count_provided;
    bool help_requested;
} CliConfig;

static volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
    keep_running = 0;
}

static CliParseResult parse_cli_args(int argc, char *argv[], CliConfig *config);

static void init_cli_config(CliConfig *config);

static void print_cli_usage(void);

int main(int argc, char *argv[]) {

    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("Error registering signal handler");
        return EXIT_FAILURE;
    }

    CliConfig cli_config;

    init_cli_config(&cli_config);


    CliParseResult parse_cli_result = parse_cli_args(argc, argv, &cli_config);

    if (parse_cli_result == CLI_PARSE_INVALID) {
        print_cli_usage();
        return EXIT_FAILURE;
    }

    if (cli_config.help_requested) {
        print_cli_usage();
        return EXIT_SUCCESS;
    }

    KvStore store;
    bool initialized;

    if (cli_config.bucket_count_provided) {
        initialized = kv_store_init_with_bucket_count(&store, cli_config.bucket_count);
    } else {
        initialized = kv_store_init(&store);
    }

    if (!initialized) {
        printf("Error: Failed to initialize KV store.\n");
        return EXIT_FAILURE;
    }

    const char *file_path = cli_config.file_path;

    load_storage_log(&store, file_path);

    char input[MAX_INPUT_SIZE];

    while (keep_running) {

        bool success = shell_read_input(input, sizeof(input));

        if (!success) {
            printf("\nExiting due to EOF.\n");
            break;
        }

        ParsedCommand command;
        ParseResult parse_result = parse_command(input, &command);

        if (parse_result == PARSE_EMPTY) {
            continue;
        }

        if (parse_result == PARSE_INVALID) {
            printf("Error: invalid command usage\n");
            continue;
        }

        CommandResult command_result = handle_command(&store, &command, file_path);

        if (command_result == COMMAND_EXIT) {
            break;
        } else if (command_result == COMMAND_SUCCESS) {
            continue;
        } else if (command_result == COMMAND_UNKNOWN) {
            printf("Error: Command '%s' not recognized. Type 'help'.\n", input);
        } else if (command_result == COMMAND_ERROR) {
            continue;
        } else {
            printf("Error: Unknown error occurred.\n");
        }
    }

    kv_store_destroy(&store);

    return EXIT_SUCCESS;
}

static CliParseResult parse_cli_args(int argc, char *argv[], CliConfig *config) {
    const char *bucket_count_option = "--bucket-count";
    bool file_path_provided = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            if (argc != 2) {
                return CLI_PARSE_INVALID;
            }

            config->help_requested = true;
            return CLI_PARSE_OK;
        }

        if (strcmp(argv[i], bucket_count_option) == 0) {
            if (config->bucket_count_provided) {
                return CLI_PARSE_INVALID;
            }

            if (i + 1 >= argc) {
                return CLI_PARSE_INVALID;
            }

            char *endptr = NULL;
            errno = 0;

            long value = strtol(argv[i + 1], &endptr, 10);

            if (errno == ERANGE || endptr == argv[i + 1] || *endptr != '\0') {
                return CLI_PARSE_INVALID;
            }

            if (value <= 0) {
                return CLI_PARSE_INVALID;
            }

            config->bucket_count = (size_t)value;
            config->bucket_count_provided = true;

            i++;
        } else {
            if (file_path_provided) {
                return CLI_PARSE_INVALID;
            }

            config->file_path = argv[i];
            file_path_provided = true;
        }
    }

    return CLI_PARSE_OK;
}

static void init_cli_config(CliConfig *config) {
    config->file_path = "../data/pagedb.data";
    config->bucket_count = DEFAULT_BUCKET_COUNT;
    config->bucket_count_provided = false;
    config->help_requested = false;
}

static void print_cli_usage(void) {
    printf("PageDB - Simple append-only key-value database\n");
    printf("\n");
    printf("Usage:\n");
    printf("  PageDB\n");
    printf("  PageDB [database_file]\n");
    printf("  PageDB [database_file] [--bucket-count N]\n");
    printf("  PageDB [--bucket-count N] [database_file]\n");
    printf("\n");
    printf("Options:\n");
    printf("  --bucket-count N   Set initial hash table bucket count. N must be greater than 0.\n");
    printf("  --help             Show this help message and exit.\n");
    printf("  -h                 Show this help message and exit.\n");
    printf("\n");
    printf("Defaults:\n");
    printf("  database_file      ../data/pagedb.data\n");
    printf("  bucket count       DEFAULT_BUCKET_COUNT\n");
    printf("\n");
    printf("Examples:\n");
    printf("  PageDB\n");
    printf("  PageDB mydb.data\n");
    printf("  PageDB --bucket-count 3\n");
    printf("  PageDB mydb.data --bucket-count 3\n");
    printf("  PageDB --bucket-count 3 mydb.data\n");
}
