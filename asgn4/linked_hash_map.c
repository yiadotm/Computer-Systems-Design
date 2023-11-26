#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_hash_map.h"

#define BASE 256
#define PRIME 101

//referenced ChatGPT for linked hash map (stated in README)


unsigned int polynomial_hash(const char *str) {
    unsigned int hash_value = 0;

    while (*str != '\0') {
        hash_value = (hash_value * BASE + *str) % PRIME;
        str++;
    }

    return hash_value;
}

LinkedHashMap *create_linkedHashMap(size_t capacity, PRIORITY lock_priority, uint32_t n) {
    LinkedHashMap *map = (LinkedHashMap *)malloc(sizeof(LinkedHashMap));
    if (!map) {
        perror("Error initializing linked hash map");
        exit(EXIT_FAILURE);
    }

    map->capacity = capacity;
    map->buckets = (Node **)calloc(capacity, sizeof(Node *));
    if (!map->buckets) {
        perror("Error initializing linked hash map buckets");
        exit(EXIT_FAILURE);
    }
    map->lock = rwlock_new(lock_priority, n);

    return map;
}

void linkedHashMap_put(LinkedHashMap *map, const char *key, int value) {
    writer_lock(map->lock);
    unsigned int hash_value = polynomial_hash(key);
    unsigned int index = hash_value % map->capacity;

    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node) {
        perror("Error inserting key-value pair");
        exit(EXIT_FAILURE);
    }

    new_node->key = strdup(key);
    new_node->value = value;
    new_node->next = map->buckets[index];
    map->buckets[index] = new_node;
    writer_unlock(map->lock);
}

int linkedHashMap_get(LinkedHashMap *map, const char *key) {
    reader_lock(map->lock);
    unsigned int hash_value = polynomial_hash(key);
    unsigned int index = hash_value % map->capacity;

    Node *current = map->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    reader_unlock(map->lock);
    // Key not found
    return -1;
}

void linkedHashMap_destroy(LinkedHashMap *map) {
    for (size_t i = 0; i < map->capacity; i++) {
        Node *current = map->buckets[i];
        while (current != NULL) {
            Node *next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }
    rwlock_delete(map->lock);
    free(map->buckets);
    free(map);
}

