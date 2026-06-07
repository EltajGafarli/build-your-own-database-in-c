#include "../include/kv_store.h"

#include <stdlib.h>
#include <string.h>

static KvEntry create_kv_entry(const char *key, const char *value);

static void copy_token(char *destination, const char *source);

void kv_store_init(KvStore *store) {
    store->size = 0;
    for (size_t i = 0; i < KV_MAX_ITEMS; i++) {
        store->entries[i].in_use = false;
        store->entries[i].key[0] = '\0';
        store->entries[i].value[0] = '\0';
    }
}

KvResult kv_store_put(KvStore *store, const char *key, const char *value) {
    for (size_t i = 0; i < KV_MAX_ITEMS; i++) {
        if (store->entries[i].in_use) {
            if (strcmp(store->entries[i].key, key) == 0) {
                copy_token(store->entries[i].value, value);
                return KV_OK;
            }
        }
    }


    for (size_t i = 0; i < KV_MAX_ITEMS; i++) {
        if (!store->entries[i].in_use) {
            store->entries[i] = create_kv_entry(key, value);
            store->size++;
            return KV_OK;
        }
    }

    return KV_FULL;
}


static KvEntry create_kv_entry(const char *key, const char *value) {
    KvEntry entry;

    copy_token(entry.key, key);

    copy_token(entry.value, value);

    entry.in_use = true;

    return entry;
}

KvResult kv_store_get(const KvStore *store, const char *key, const char **value) {
    for (size_t i = 0; i < KV_MAX_ITEMS; i++) {
        if (store->entries[i].in_use && strcmp(store->entries[i].key, key) == 0) {
            *value = store->entries[i].value;
            return KV_OK;
        }
    }

    return KV_NOT_FOUND;
}

KvResult kv_store_delete(KvStore *store, const char *key) {
    for (size_t i = 0; i < KV_MAX_ITEMS; i++) {
        if (store->entries[i].in_use && strcmp(store->entries[i].key, key) == 0) {
            store->entries[i].in_use = false;
            store->entries[i].key[0] = '\0';
            store->entries[i].value[0] = '\0';
            store->size--;
            return KV_OK;
        }
    }

    return KV_NOT_FOUND;
}

static void copy_token(char *destination, const char *source) {
    strncpy(destination, source, MAX_TOKEN_SIZE - 1);
    destination[MAX_TOKEN_SIZE - 1] = '\0';
}
