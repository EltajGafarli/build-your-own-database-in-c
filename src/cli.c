#include "../include/cli.h"
#include "../include/kv_store.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static CliParseResult parse_cli_args(int argc, char *argv[], CliConfig *config);
static void print_cli_usage(void);

void init_cli_config(CliConfig *config) {
    config->file_path = "../data/pagedb.data";
    config->bucket_count = DEFAULT_BUCKET_COUNT;
    config->bucket_count_provided = false;
    config->help_requested = false;
}

CliExecuteResult handle_cli_args(int argc, char *argv[], CliConfig *config) {
    CliParseResult parse_cli_result = parse_cli_args(argc, argv, config);

    if (parse_cli_result == CLI_PARSE_INVALID) {
        print_cli_usage();
        return CLI_EXECUTE_EXIT_FAILURE;
    }

    if (config->help_requested) {
        print_cli_usage();
        return CLI_EXECUTE_EXIT_SUCCESS;
    }

    return CLI_EXECUTE_OK;
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
