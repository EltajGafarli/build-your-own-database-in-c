#include<assert.h>
#include<string.h>
#include<stdio.h>
#include<stdbool.h>
#include "../../include/cli.h"
#include "../../include/kv_store.h"

#define RUN_TEST(test_func)             \
do {                                                         \
printf("[ RUN      ] %s\n", #test_func);                 \
fflush(stdout);                                          \
test_func();                                             \
printf("[       OK ] %s\n", #test_func);                 \
fflush(stdout);                                          \
} while (0)

void test_init_cli_config__called__sets_default_values() {
    CliConfig config;
    init_cli_config(&config);

    assert(config.file_path != NULL);
    assert(config.bucket_count == DEFAULT_BUCKET_COUNT);
    assert(config.bucket_count_provided == false);
    assert(config.help_requested == false);
    assert(strcmp(config.file_path, "../data/pagedb.data") == 0);
}

void test_handle_cli_args__bucket_count_option__sets_bucket_count() {
    CliConfig config;
    init_cli_config(&config);

    int argc = 3;
    char *argv_pagedb_bucket_count[] = {"pagedb", "--bucket-count", "100"};


    CliExecuteResult result = handle_cli_args(argc, argv_pagedb_bucket_count, &config);

    assert(result == CLI_EXECUTE_OK);
}

void test_handle_cli_args__help_option__returns_exit_success() {
    CliConfig config;
    init_cli_config(&config);
    int argc = 2;
    char *argv_pagedb_help[] = {"pagedb", "--help"};
    CliExecuteResult result = handle_cli_args(argc, argv_pagedb_help, &config);
    assert(result == CLI_EXECUTE_EXIT_SUCCESS);
}

void test_handle_cli_args_dash_h_option__returns_exit_success() {
    CliConfig config;
    init_cli_config(&config);
    int argc = 2;
    char *argv_pagedb_dash_h[] = {"pagedb", "-h"};
    CliExecuteResult result = handle_cli_args(argc, argv_pagedb_dash_h, &config);

    assert(result == CLI_EXECUTE_EXIT_SUCCESS);
}

void test_handle_cli_args__invalid_bucket_count__returns_exit_failure() {
    CliConfig config;
    init_cli_config(&config);
    int argc = 3;
    char *argv_pagedb_invalid_bucket_count[] = {"pagedb", "--bucket-count", "invalid_bucket_count"};
    CliExecuteResult result = handle_cli_args(argc, argv_pagedb_invalid_bucket_count, &config);
    assert(result == CLI_EXECUTE_EXIT_FAILURE);
}

void test_handle_cli_args__invalid_file_path__returns_exit_failure() {
    CliConfig config;
    init_cli_config(&config);
    int argc = 3;
    char *argv_pagedb_invalid_file_path[] = {"pagedb", "--file-path", "invalid_file_path"};
    CliExecuteResult result = handle_cli_args(argc, argv_pagedb_invalid_file_path, &config);

    assert(result == CLI_EXECUTE_EXIT_FAILURE);
}

void test_handle_cli_args_no_args() {
    CliConfig config;
    init_cli_config(&config);
    int argc = 1;
    char *argv_pagedb_no_args[] = {};
    CliExecuteResult result = handle_cli_args(argc, argv_pagedb_no_args, &config);

    assert(result == CLI_EXECUTE_OK);
}

int main() {
    RUN_TEST(test_init_cli_config__called__sets_default_values);
    RUN_TEST(test_handle_cli_args__bucket_count_option__sets_bucket_count);
    RUN_TEST(test_handle_cli_args__help_option__returns_exit_success);
    RUN_TEST(test_handle_cli_args_dash_h_option__returns_exit_success);
    RUN_TEST(test_handle_cli_args__invalid_bucket_count__returns_exit_failure);
    RUN_TEST(test_handle_cli_args__invalid_file_path__returns_exit_failure);
    RUN_TEST(test_handle_cli_args_no_args);
    return 0;
}
