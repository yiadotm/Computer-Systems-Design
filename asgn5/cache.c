#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "cache.h"

void pushNode(struct Cache *cache, struct Node *node) {
    if (node && cache) {
        cache->tail->next = node;
        cache->tail = node;
    }
    cache->currentSize++;
}

struct Node *popNode(struct Cache *cache) {
    struct Node *pop = NULL;
    if (cache) {
        pop = cache->head;
        cache->head = cache->head->next;
    }
    cache->currentSize--;
    return pop;
}

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
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void updateNodeAge(struct Cache *cache, const char *item) {
    struct Node *updateNode = cache->head;
    while (strcmp(updateNode->data, item) != 0) {
        updateNode = updateNode->next;
    }
    updateNode->age = 1;
}

void evictFIFO(struct Cache *cache) {
    if (!isInList(cache->removed, cache->head->data)) {
        append(cache->removed, cache->head->data);
        // printRemovedCache(cache);
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
        // printRemovedCache(cache);
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
    if (current == NULL || current == cache->tail) {
        return;
    }

    //item is at the head
    //need to switch head and tail
    if (prev == NULL) {
        if (cache->head == NULL || cache->head == cache->tail) {
            // Cache is empty or has only one node, no need to switch
            return;
        }

        // Swap the data of the head and tail nodes
        const char *temp = strdup(cache->head->data);
        free(cache->head->data); // Free the previous data of the head
        cache->head->data = strdup(cache->tail->data);
        free(cache->tail->data); // Free the previous data of the tail
        cache->tail->data = strdup(temp);
        free((void *) temp);

        return;
    }
}

void evictClock(struct Cache *cache) {
    if (cache->head == NULL) {
        // Cache is empty, nothing to evict
        return;
    }
    struct Node *current = cache->head;
    // fprintf(stderr, "youngest age: %d\n", current->age);
    while (current != NULL && current->age > 0) {
        current->age = 0;
        struct Node *pop = popNode(cache);
        assert(current == pop);
        pushNode(cache, pop);
    }
    current = popNode(cache);
    assert(current->age == 0);

    // fprintf(stderr, "item in evict: %s\n", current->data);
    //evict item
    if (!isInList(cache->removed, current->data)) {
        append(cache->removed, current->data);
        // fprintf(stderr, "add to removed\n");
        // printRemovedCache(cache);
    }

    // fprintf(stderr, "cache after remove %s, ", current->data);

    // current = current->next;
    free(current->data);
    free(current);

    cache->currentSize--;
    // printCache(cache);
}

// Function to add an item to the cache
void addToCache(struct Cache *cache, const char *item) {

    if (cache->currentSize >= cache->size && cache->policy == C) {
        evictClock(cache);
    }

    struct Node *newNode = (struct Node *) malloc(sizeof(struct Node));
    newNode->data = strdup(item);
    newNode->age = 0;
    newNode->next = NULL;

    if (cache->currentSize == 0) {
        cache->head = newNode;
        cache->tail = newNode;
    } else {
        cache->tail->next = newNode;
        cache->tail = newNode;
    }

    cache->currentSize++;
    if (cache->policy == C) {
        return;
    }
    // If the cache is full, evict item
    if (cache->currentSize > cache->size) {
        switch (cache->policy) {
        case F: evictFIFO(cache); break;
        case L: evictLRU(cache); break;
        case C: evictClock(cache); break;
        default:
            // Unknown policy, add appropriate error handling
            fprintf(stderr, "Error: Unknown cache policy\n");
            break;
        }
    }
}

// Function to handle cache access and print HIT or MISS
void handleCacheAccess(struct Cache *cache, const char *item) {
    // fprintf(stderr, "item: %s\n", item);
    // printCache(cache);
    // printRemovedCache(cache);

    if (isInCache(cache, item)) {
        if (cache->policy == L) {
            // fprintf(stderr, "here\n");
            // printCache(cache);
            swapLRU(cache, item);
        }
        if (cache->policy == C) {

            updateNodeAge(cache, item);
        }
        printf("HIT\n");
        // fprintf(stderr, "HIT\n");

        // fprintf(stderr, "item: %s, CO: %d\n", item, cache->CO);
    } else {
        // printCache(cache);
        if (isInList(cache->removed, item) && !isInCache(cache, item)) {

            cache->CA++;
            // fprintf(stderr, "CA: %d\n", cache->CA);

        } else if (!isInList(cache->removed, item) && !isInCache(cache, item)) {
            cache->CO++;
            // fprintf(stderr, "CO: %d\n", cache->CO);
        }
        // fprintf(stderr, "item in list: %s\n", item);
        // printRemovedCache(cache);

        printf("MISS\n");
        addToCache(cache, item);
        // fprintf(stderr, "MISS\n");
    }
    // fprintf(stderr, "\n");
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
