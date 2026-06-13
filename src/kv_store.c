#include "../include/kv_store.h"

#include <stdlib.h>
#include <string.h>

static KvEntry *create_kv_entry(const char *key, const char *value);

static void copy_token(char *destination, const char *source);

static unsigned int hash(const char *str);

void kv_store_init(KvStore *store) {
    store->size = 0;

    for (int i = 0; i < DEFAULT_BUCKET_SIZE; i++) {
        store->buckets[i] = NULL;
    }
}

KvResult kv_store_put(KvStore *store, const char *key, const char *value) {

    unsigned int hash_val = hash(key);

    size_t index = hash_val % DEFAULT_BUCKET_SIZE;

    if (store->buckets[index] == NULL) {
        KvEntry *entry = create_kv_entry(key, value);
        if (entry == NULL) {
            return KV_FULL;
        }
        store->buckets[index] = entry;
        store->size++;
        return KV_OK;
    } else {
        KvEntry *head = store->buckets[index];

        while (head->next != NULL) {
            if (strcmp(head->key, key) == 0) {
                copy_token(head->value, value);
                return KV_OK;
            }
            head = head->next;
        }

        if (strcmp(head->key, key) == 0) {
            copy_token(head->value, value);
            return KV_OK;
        }

        KvEntry *entry = create_kv_entry(key, value);

        if (entry == NULL) {
            return KV_FULL;
        }

        head->next = entry;
        store->size++;
        return KV_OK;
    }
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
    size_t index = hash_val % DEFAULT_BUCKET_SIZE;

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

    size_t index = hash_val % DEFAULT_BUCKET_SIZE;

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
    for (size_t i = 0; i < DEFAULT_BUCKET_SIZE; i++) {
        KvEntry *head = store->buckets[i];
        while (head != NULL) {
            KvEntry *temp = head->next;
            free(head);
            head = temp;
        }
    }

    kv_store_init(store);
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