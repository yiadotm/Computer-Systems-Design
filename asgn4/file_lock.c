#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "file_lock.h"

FileLock *file_lock_new(size_t capacity) {
    FileLock *file_lock = (FileLock *) malloc(sizeof(FileLock));
    if (file_lock == NULL) {
        // Handle allocation failure
        return NULL;
    }
    file_lock->size = capacity;
    file_lock->counter = 0;
    file_lock->map = create_linkedHashMap(capacity);
    if (file_lock->map == NULL) {
        // Handle map creation failure
        free(file_lock);
        return NULL;
    }
    file_lock->rwlock = (rwlock_t **) malloc(sizeof(rwlock_t *) * capacity);
    for (size_t i = 0; i < capacity; i++) {
        file_lock->rwlock[i] = rwlock_new(N_WAY, 1);
    }
    int rc = pthread_mutex_init(&file_lock->mutex, NULL);
    assert(!rc);
    return file_lock;
}

void file_lock_delete(FileLock **file_lock) {
    if (*file_lock != NULL) {
        linkedHashMap_destroy((*file_lock)->map);
        for (int i = 0; i < (*file_lock)->size; i++) {
            rwlock_delete(&(*file_lock)->rwlock[i]);
        }
        int rc = pthread_mutex_destroy(&(*file_lock)->mutex);
        assert(!rc);
        free((*file_lock)->rwlock);
        free(*file_lock);
        *file_lock = NULL;
    }
}
void file_lock_read_lock(FileLock *file_lock, const char *uri) {
    int lock = linkedHashMap_get(file_lock->map, uri);
    if (lock == -1) {
        pthread_mutex_lock(&file_lock->mutex);

        lock = file_lock->counter;
        reader_lock(file_lock->rwlock[lock]);
        linkedHashMap_put(file_lock->map, uri, lock);
        file_lock->counter = (file_lock->counter + 1) % file_lock->size;
        pthread_mutex_unlock(&file_lock->mutex);

    } else {
        reader_lock(file_lock->rwlock[lock]);
    }
}
void file_lock_read_unlock(FileLock *file_lock, const char *uri) {
    // pthread_mutex_lock(&file_lock->mutex);
    int lock = linkedHashMap_get(file_lock->map, uri);
    if (lock == -1) {
        return;
    }

    reader_unlock(file_lock->rwlock[lock]);
    // pthread_mutex_unlock(&file_lock->mutex);
}
void file_lock_write_lock(FileLock *file_lock, const char *uri) {

    int lock = linkedHashMap_get(file_lock->map, uri);
    if (lock == -1) {
        pthread_mutex_lock(&file_lock->mutex);

        lock = file_lock->counter;

        writer_lock(file_lock->rwlock[lock]);
        linkedHashMap_put(file_lock->map, uri, lock);
        file_lock->counter = (file_lock->counter + 1) % file_lock->size;
        pthread_mutex_unlock(&file_lock->mutex);

    } else {
        writer_lock(file_lock->rwlock[lock]);
    }
}

void file_lock_write_unlock(FileLock *file_lock, const char *uri) {
    // pthread_mutex_lock(&file_lock->mutex);
    int lock = linkedHashMap_get(file_lock->map, uri);
    if (lock == -1) {
        return;
    }
    writer_unlock(file_lock->rwlock[lock]);
    // pthread_mutex_unlock(&file_lock->mutex);
}
