#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#define F              1
#define L              2
#define C              3
#define INVALID_POLICY -1
struct Node {
    char *data;
    struct Node *next;
};

// Cache structure
struct Cache {
    struct Node *head;
    struct Node *tail;
    List removed;

    int size;
    int currentSize;
    int CO;
    int CA;
    int policy;
};

void initializeCache(struct Cache *cache, int size, int policy);

int isInCache(struct Cache *cache, const char *item);

// int isInSet(struct Cache *cache, const char *item);

void evictFIFO(struct Cache *cache);

void evictLRU(struct Cache *cache);

void addToCache(struct Cache *cache, const char *item);

void handleCacheAccess(struct Cache *cache, const char *item);

void freeCache(struct Cache *cache);

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// // Node structure for linked list representing cache
// struct Node {
//     char* data;
//     struct Node* next;
// };

// // Cache structure
// struct Cache {
//     struct Node* head;
//     struct Node* tail;
//     int size;
//     int currentSize;
// };

// // Function to initialize the cache
// void initializeCache(struct Cache* cache, int size) {
//     cache->head = NULL;
//     cache->tail = NULL;
//     cache->size = size;
//     cache->currentSize = 0;
// }

// // Function to check if an item is in the cache
// int isInCache(struct Cache* cache, const char* item) {
//     struct Node* current = cache->head;
//     while (current != NULL) {
//         if (strcmp(current->data, item) == 0) {
//             return 1; // Found in cache (HIT)
//         }
//         current = current->next;
//     }
//     return 0; // Not found in cache (MISS)
// }

// // Function to add an item to the cache
// void addToCache(struct Cache* cache, const char* item) {
//     struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
//     newNode->data = strdup(item);
//     newNode->next = NULL;

//     if (cache->currentSize == 0) {
//         cache->head = newNode;
//         cache->tail = newNode;
//     } else {
//         cache->tail->next = newNode;
//         cache->tail = newNode;
//     }

//     cache->currentSize++;

//     // If the cache is full, evict the first item (FIFO)
//     if (cache->currentSize > cache->size) {
//         struct Node* temp = cache->head;
//         cache->head = temp->next;
//         free(temp->data);
//         free(temp);
//         cache->currentSize--;
//     }
// }

// // Function to handle cache access and print HIT or MISS
// void handleCacheAccess(struct Cache* cache, const char* item) {
//     if (isInCache(cache, item)) {
//         printf("HIT\n");
//     } else {
//         printf("MISS\n");
//         addToCache(cache, item);
//     }
// }

// // Function to free memory allocated for the cache
// void freeCache(struct Cache* cache) {
//     struct Node* current = cache->head;
//     while (current != NULL) {
//         struct Node* temp = current;
//         current = current->next;
//         free(temp->data);
//         free(temp);
//     }
// }
