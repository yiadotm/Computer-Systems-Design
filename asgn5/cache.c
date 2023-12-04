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
    cache->CO = 0;
    cache->CA = 0;
}

void printRemovedCache(struct Cache *cache) {
    fprintf(stderr, "List size: %d, List: ", length(cache->removed));
    printList(stderr, cache->removed);
    fprintf(stderr, "\n");
}

void printCache(struct Cache *cache) {
    fprintf(stderr, "Cache: ");
    struct Node *N = cache->head;
    while (N != NULL) {
        fprintf(stderr, "%s ", N->data);
        N = N->next;
    }
    fprintf(stderr, "\n");
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

void evictFIFO(struct Cache *cache) {
    if (!isInList(cache->removed, cache->head->data)) {
        append(cache->removed, cache->head->data);
        printRemovedCache(cache);
    }
    struct Node *temp = cache->head;
    cache->head = temp->next;
    free(temp->data);
    free(temp);
    cache->currentSize--;
}
void evictLRU(struct Cache *cache) {
    if (!isInList(cache->removed, cache->head->data)) {
        append(cache->removed, cache->head->data);
        printRemovedCache(cache);
    }
    struct Node *temp = cache->head;
    cache->head = temp->next;
    free(temp->data);
    free(temp);
    cache->currentSize--;
}
void swapLRU(struct Cache *cache, const char *item) {
    // If the cache is empty or has only one node, no need to move
    if (cache->head == NULL || cache->head == cache->tail) {
        return;
    }

    struct Node *prev = NULL;
    struct Node *current = cache->head;

    // Find the node containing the item
    while (current != NULL && strcmp(current->data, item) != 0) {
        prev = current;
        current = current->next;
    }

    // If the item is not in the cache, or it's already the tail, no need to move
    if (current == NULL || current == cache->tail || prev == NULL) {
        return;
    }

    //item is at the head
    //need to switch head and tail
    if (prev == NULL) {
        if (cache->head == NULL || cache->head == cache->tail) {
            // Cache is empty or has only one node, no need to switch
            return;
        }

        struct Node *tailPrev = NULL;
        struct Node *current = cache->head;

        // Traverse the list to find tail prev
        while (current->next != cache->tail) {
            tailPrev = current;
            current = current->next;
        }

        // Update pointers to switch head and tail
        tailPrev->next = cache->head;
        cache->head = cache->tail;
        cache->tail->next = NULL; // Disconnect old tail from the rest of the list
        cache->tail = tailPrev;
        return;
    }
    // Update pointers to move the node containing the item to the tail
    // if (prev != NULL) {
    prev->next = current->next;
    // }
    cache->tail->next = current;
    current->next = NULL;
    cache->tail = current;
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

    // If the cache is full, evict item
    if (cache->currentSize > cache->size) {
        switch (cache->policy) {
        case F: evictFIFO(cache); break;
        case L: evictLRU(cache); break;

        default:
            // Unknown policy, add appropriate error handling
            fprintf(stderr, "Error: Unknown cache policy\n");
            break;
        }
    }
}
// case C:
//     evictClock(cache, item);
//     break;
// Function to handle cache access and print HIT or MISS
void handleCacheAccess(struct Cache *cache, const char *item) {
    fprintf(stderr, "item: %s\n", item);
    printCache(cache);
    if (isInCache(cache, item)) {
        if (cache->policy == L) {
            fprintf(stderr, "here\n");
            // printCache(cache);
            swapLRU(cache, item);
        }
        printf("HIT\n");
        cache->CO++;
        // fprintf(stderr, "item: %s, CO: %d\n", item, cache->CO);
    } else {
        // printCache(cache);
        if (isInList(cache->removed, item)) {

            cache->CA++;
        }
        // fprintf(stderr, "item in list: %s\n", item);
        // printRemovedCache(cache);

        printf("MISS\n");
        addToCache(cache, item);

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
