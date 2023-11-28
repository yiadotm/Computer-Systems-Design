#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rwlock.h"

// Structure for a hash map node
typedef struct Node {
    char *key;
    int value;
    struct Node *next;
} Node;

typedef struct LinkedHashMap {
    size_t size;
    size_t capacity;
    Node **buckets;
    rwlock_t *lock;
} LinkedHashMap;

unsigned int polynomial_hash(const char *str);

LinkedHashMap *create_linkedHashMap(size_t capacity);

void linkedHashMap_resize(LinkedHashMap *map, size_t new_capacity);

void linkedHashMap_put(LinkedHashMap *map, const char *key, int value);

int linkedHashMap_get(LinkedHashMap *map, const char *key);

void linkedHashMap_destroy(LinkedHashMap *map);
