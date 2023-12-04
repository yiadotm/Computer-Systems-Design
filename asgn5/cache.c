#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cache.h"

// Function to initialize the cache
void initializeCache(struct Cache *cache, int size, int policy) {
    cache->head = NULL;
    cache->tail = NULL;
    cache->policy = policy;
    cache->size = size;
    cache->currentSize = 0;
    cache->removed = newList();
}

// Function to check if an item is in the cache
int isInCache(struct Cache *cache, const char *item) {
    struct Node *current = cache->head;
    while (current != NULL) {
        if (strcmp(current->data, item) == 0) {
            return 1; // Found in cache (HIT)
        }
        current = current->next;
    }
    return 0; // Not found in cache (MISS)
}

// Function to add an item to the cache
void addToCache(struct Cache *cache, const char *item) {
    struct Node *newNode = (struct Node *) malloc(sizeof(struct Node));
    newNode->data = strdup(item);
    newNode->next = NULL;

    if (cache->currentSize == 0) {
        cache->head = newNode;
        cache->tail = newNode;
    } else {
        cache->tail->next = newNode;
        cache->tail = newNode;
    }

    cache->currentSize++;

    // If the cache is full, evict the first item (FIFO)
    if (cache->currentSize > cache->size) {
        append(cache->removed, item);
        struct Node *temp = cache->head;
        cache->head = temp->next;
        free(temp->data);
        free(temp);
        cache->currentSize--;
    }
}

// Function to handle cache access and print HIT or MISS
void handleCacheAccess(struct Cache *cache, const char *item) {
    if (isInCache(cache, item)) {
        printf("HIT\n");
        cache->CO++;
    } else {
        if (isInList(cache->removed, item)) {
            cache->CA++;
        }
        printf("MISS\n");
        addToCache(cache, item);
        fprintf(stderr, "item: %s\n", item);
        // cache->CO++;
    }
}

// Function to free memory allocated for the cache
void freeCache(struct Cache *cache) {
    struct Node *current = cache->head;
    while (current != NULL) {
        struct Node *temp = current;
        current = current->next;
        free(temp->data);
        free(temp);
    }
    freeList(&cache->removed);
    cache->head = NULL;
    cache->tail = NULL;
}
