#pragma once

#include <stdbool.h>
#include <stddef.h>

#define KV_MAX_ITEMS 1024
#define MAX_TOKEN_SIZE 256
#define DEFAULT_BUCKET_SIZE 256

typedef enum {
    KV_OK,
    KV_NOT_FOUND,
    KV_FULL
} KvResult;

typedef struct KvEntry {
    char key[MAX_TOKEN_SIZE];
    char value[MAX_TOKEN_SIZE];
    struct KvEntry *next;
} KvEntry;

typedef struct {
    KvEntry* buckets[DEFAULT_BUCKET_SIZE];
    size_t size;
} KvStore;

void kv_store_init(KvStore *store);

KvResult kv_store_put(KvStore *store, const char *key, const char *value);

KvResult kv_store_get(const KvStore *store, const char *key, const char **value);

KvResult kv_store_delete(KvStore *store, const char *key);

void kv_store_destroy(KvStore *store);