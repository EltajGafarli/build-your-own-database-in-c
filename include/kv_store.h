#pragma once

#include <stdbool.h>
#include <stddef.h>

#define KV_MAX_ITEMS 1024
#define MAX_TOKEN_SIZE 256
#define DEFAULT_BUCKET_COUNT 256

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
    KvEntry **buckets;
    size_t size;
    size_t bucket_count;
} KvStore;

typedef struct KvStoreStats {

    size_t active_records;
    size_t bucket_count;
    size_t used_buckets;
    size_t collision_count;
    size_t max_records;
    size_t max_bucket_depth;
    double load_factor;
} KvStoreStats;

bool kv_store_init(KvStore *store);

KvResult kv_store_put(KvStore *store, const char *key, const char *value);

KvResult kv_store_get(const KvStore *store, const char *key, const char **value);

KvResult kv_store_delete(KvStore *store, const char *key);

void kv_store_destroy(KvStore *store);

void kv_store_get_stats(const KvStore *store, KvStoreStats *stats);

void kv_store_stats_init(KvStoreStats *stats);