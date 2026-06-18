#include "../include/kv_store.h"

#include <stdlib.h>
#include <string.h>

static KvEntry *create_kv_entry(const char *key, const char *value);

static void copy_token(char *destination, const char *source);

static unsigned int hash(const char *str);

static size_t get_used_bucket_count(const KvStore *store);

static size_t get_collision_count(const KvStore *store);

static size_t get_max_bucket_depth(const KvStore *store);

static bool kv_store_resize(KvStore *store, size_t new_bucket_count);

bool kv_store_init(KvStore *store) {
    store->size = 0;
    store->bucket_count = DEFAULT_BUCKET_COUNT;

    store->buckets = calloc(store->bucket_count, sizeof(KvEntry *));

    if (store->buckets == NULL) {
        store->size = 0;
        store->bucket_count = 0;
        return false;
    }

    return true;
}

bool kv_store_init_with_bucket_count(KvStore *store, size_t bucket_count) {
    store->size = 0;
    store->bucket_count = bucket_count;

    store->buckets = calloc(store->bucket_count, sizeof(KvEntry *));

    if (store->buckets == NULL) {
        store->size = 0;
        store->bucket_count = 0;
        return false;
    }

    return true;
}

KvResult kv_store_put(KvStore *store, const char *key, const char *value) {

    unsigned int hash_val = hash(key);

    size_t index = hash_val % store->bucket_count;

    KvEntry *head = store->buckets[index];

    while (head != NULL) {
        if (strcmp(head->key, key) == 0) {
            copy_token(head->value, value);
            return KV_OK;
        }
        head = head->next;
    }

    double next_load_factor = (double)(store->size + 1) / (double)store->bucket_count;

    if (next_load_factor > MAX_LOAD_FACTOR) {
        if (!kv_store_resize(store, store->bucket_count * 2)) {
            return KV_FULL;
        }

        index = hash_val % store->bucket_count;
    }

    KvEntry *entry = create_kv_entry(key, value);

    if (entry == NULL) {
        return KV_FULL;
    }

    entry->next = store->buckets[index];
    store->buckets[index] = entry;
    store->size++;

    return KV_OK;
}


static KvEntry *create_kv_entry(const char *key, const char *value) {
    KvEntry *entry = malloc(sizeof(KvEntry));

    if (entry == NULL) {
        return NULL;
    }

    copy_token(entry->key, key);

    copy_token(entry->value, value);

    entry->next = NULL;

    return entry;
}

KvResult kv_store_get(const KvStore *store, const char *key, const char **value) {

    unsigned int hash_val = hash(key);
    size_t index = hash_val % store->bucket_count;

    KvEntry *head = store->buckets[index];

    while (head != NULL) {
        if (strcmp(head->key, key) == 0) {
            *value = head->value;
            return KV_OK;
        }
        head = head->next;
    }

    return KV_NOT_FOUND;
}

KvResult kv_store_delete(KvStore *store, const char *key) {

    unsigned int hash_val = hash(key);

    size_t index = hash_val % store->bucket_count;

    KvEntry *head = store->buckets[index];

    KvEntry *prev = NULL;

    while (head != NULL) {
        if (strcmp(head->key, key) == 0) {
            break;
        }
        prev = head;
        head = head->next;
    }

    if (head == NULL) {
        return KV_NOT_FOUND;
    }

    if (prev == NULL) {
        store->buckets[index] = head->next;
        free(head);
        store->size--;
    } else {
        KvEntry *temp = prev->next;
        prev->next = prev->next->next;
        free(temp);
        store->size--;
    }

    return KV_OK;

}

void kv_store_destroy(KvStore *store) {
    for (size_t i = 0; i < store->bucket_count; i++) {
        KvEntry *head = store->buckets[i];
        while (head != NULL) {
            KvEntry *temp = head->next;
            free(head);
            head = temp;
        }
    }

    free(store->buckets);
    store->size = 0;
    store->bucket_count = 0;
    store->buckets = NULL;
}

void kv_store_get_stats(const KvStore *store, KvStoreStats *stats) {

    kv_store_stats_init(stats);

    stats->active_records = store->size;
    stats->max_records = KV_MAX_ITEMS;
    stats->bucket_count = store->bucket_count;
    stats->used_buckets = get_used_bucket_count(store);
    stats->collision_count = get_collision_count(store);
    stats->load_factor = (double) store->size / (double)store->bucket_count;
    stats->max_bucket_depth = get_max_bucket_depth(store);
}

void kv_store_stats_init(KvStoreStats *stats) {
    stats->active_records = 0;
    stats->max_records = 0;
    stats->bucket_count = 0;
    stats->used_buckets = 0;
    stats->collision_count = 0;
    stats->max_bucket_depth = 0;
    stats->load_factor = 0.0;
}

static size_t get_used_bucket_count(const KvStore *store) {
    size_t count = 0;

    for (size_t i = 0; i < store->bucket_count; i++) {
        if (store->buckets[i] != NULL) {
            count++;
        }
    }

    return count;
}

static size_t get_max_bucket_depth(const KvStore *store) {
    size_t max_depth = 0;

    for (size_t i = 0; i < store->bucket_count; i++) {
        if (store->buckets[i] != NULL) {

            size_t current_depth = 0;
            KvEntry *head = store->buckets[i];
            while (head != NULL) {
                head = head->next;
                current_depth++;
            }

            max_depth = current_depth > max_depth ? current_depth : max_depth;
        }
    }

    return max_depth;
}

static size_t get_collision_count(const KvStore *store) {

    size_t count = 0;

    for (size_t i = 0; i < store->bucket_count; i++) {
        if (store->buckets[i] != NULL) {
            int current_count = 0;
            KvEntry *head = store->buckets[i];
            while (head != NULL) {
                head = head->next;
                current_count++;
            }

            count += current_count > 1 ? current_count - 1 : 0;
        }

    }

    return count;
}

static void copy_token(char *destination, const char *source) {
    strncpy(destination, source, MAX_TOKEN_SIZE - 1);
    destination[MAX_TOKEN_SIZE - 1] = '\0';
}

static unsigned int hash(const char *str) {
    const unsigned char *p = (const unsigned char *) str;
    unsigned int hashVal = 0;

    while (*p != '\0') {
        hashVal = (hashVal << 5) + *p;
        p++;
    }

    return hashVal;
}

static bool kv_store_resize(KvStore *store, size_t new_bucket_count) {

    KvEntry **new_buckets = calloc(new_bucket_count, sizeof(KvEntry *));

    KvEntry **old_buckets = store->buckets;

    if (new_buckets == NULL) {
        return false;
    }

    for (size_t i = 0; i < store->bucket_count; i++) {
        KvEntry *current = old_buckets[i];

        while (current != NULL) {
            KvEntry *next = current->next;

            size_t new_index = hash(current->key) % new_bucket_count;

            current->next = new_buckets[new_index];
            new_buckets[new_index] = current;
            current = next;
        }
    }

    free(old_buckets);

    store->buckets = new_buckets;
    store->bucket_count = new_bucket_count;

    return true;
}