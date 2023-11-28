#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "rwlock.h"
#include "linked_hash_map.h"
#include <pthread.h>

typedef struct {
    int size;
    int counter;
    LinkedHashMap *map;
    rwlock_t **rwlock;
    pthread_mutex_t mutex;
} FileLock;

FileLock *file_lock_new(size_t capacity);

void file_lock_delete(FileLock **file_lock);

void file_lock_read_lock(FileLock *file_lock, const char *uri);

void file_lock_read_unlock(FileLock *file_lock, const char *uri);

void file_lock_write_lock(FileLock *file_lock, const char *uri);

void file_lock_write_unlock(FileLock *file_lock, const char *uri);
