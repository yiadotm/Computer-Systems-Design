#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_hash_map.h"

#define BASE  256
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

LinkedHashMap *create_linkedHashMap(size_t capacity) {
    LinkedHashMap *map = (LinkedHashMap *) malloc(sizeof(LinkedHashMap));
    if (!map) {
        perror("Error initializing linked hash map");
        exit(EXIT_FAILURE);
    }
    map->size = 0;
    map->capacity = capacity;
    map->buckets = (Node **) calloc(capacity, sizeof(Node *));
    if (!map->buckets) {
        perror("Error initializing linked hash map buckets");
        free(map);
        exit(EXIT_FAILURE);
    }

    return map;
}
void linkedHashMap_resize(LinkedHashMap *map, size_t new_capacity) {
    Node **new_buckets = (Node **) calloc(new_capacity, sizeof(Node *));
    if (!new_buckets) {
        perror("Error resizing linked hash map buckets");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < map->capacity; i++) {
        Node *current = map->buckets[i];
        while (current != NULL) {
            Node *next = current->next;

            unsigned int hash_value = polynomial_hash(current->key);
            unsigned int index = hash_value % new_capacity;

            current->next = new_buckets[index];
            new_buckets[index] = current;

            current = next;
        }
    }

    free(map->buckets);
    map->buckets = new_buckets;
    map->capacity = new_capacity;
}

void linkedHashMap_put(LinkedHashMap *map, const char *key, int value) {
    if (map->size >= map->capacity) {
        // Resize the map if it's full (you can choose a different resizing strategy)
        linkedHashMap_resize(map, 2 * map->capacity);
    }
    unsigned int hash_value = polynomial_hash(key);
    unsigned int index = hash_value % map->capacity;

    Node *new_node = (Node *) malloc(sizeof(Node));
    if (!new_node) {
        perror("Error inserting key-value pair");
        exit(EXIT_FAILURE);
    }

    new_node->key = strdup(key);
    new_node->value = value;
    new_node->next = map->buckets[index];
    map->buckets[index] = new_node;
    map->size++;
}

int linkedHashMap_get(LinkedHashMap *map, const char *key) {
    unsigned int hash_value = polynomial_hash(key);
    unsigned int index = hash_value % map->capacity;

    Node *current = map->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
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
    free(map->buckets);
    free(map);
}
