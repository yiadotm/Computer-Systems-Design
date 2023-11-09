#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <assert.h>
#include "rwlock.h"
#define UNUSED(x) (void) x

static int reader_should_wait(rwlock_t *rw);
static int writer_should_wait(rwlock_t *rw);

typedef struct rwlock rwlock_t;
typedef struct rwlock {
    PRIORITY p;
    uint32_t n;
    int activeReaders;
    int activeWriters;
    int waitingReaders;
    int waitingWriters;
    pthread_mutex_t mutex;
    pthread_cond_t readGo;
    pthread_cond_t writeGo;

} rwlock_t;

rwlock_t *rwlock_new(PRIORITY p, uint32_t n) {
    rwlock_t *r = (rwlock_t *) malloc(sizeof(rwlock_t));
    r->p = p;
    if (p == N_WAY) {
        r->n = n;
    } else {
        r->n = -1;
    }
    int rc = 0;
    rc = pthread_mutex_init(&r->mutex, NULL);
    assert(!rc);
    rc = pthread_cond_init(&r->readGo, NULL);
    assert(!rc);
    rc = pthread_cond_init(&r->writeGo, NULL);
    assert(!rc);
    return r;
}

void rwlock_delete(rwlock_t **rw) {
    if (rw != NULL) {
        int rc = 0;
        rc = pthread_mutex_destroy(&(*rw)->mutex);
        rc = pthread_cond_destroy(&(*rw)->readGo);
        rc = pthread_cond_destroy(&(*rw)->writeGo);
        assert(!rc);
        free(*rw);
        *rw = NULL;
    }
}

void reader_lock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->mutex);
    rw->waitingReaders += 1;
    while (reader_should_wait(rw)) {
        pthread_cond_wait(&rw->readGo, &rw->mutex);
    }
    rw->waitingReaders -= 1;
    rw->activeReaders += 1;

    pthread_mutex_unlock(&rw->mutex);
}

void reader_unlock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->mutex);
    rw->activeReaders -= 1;

    if (rw->activeReaders == 0 && rw->waitingWriters > 0) {
        pthread_cond_signal(&rw->writeGo);
    }
    pthread_mutex_unlock(&rw->mutex);
}

void writer_lock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->mutex);
    rw->waitingWriters += 1;
    while (writer_should_wait(rw)) {
        pthread_cond_wait(&rw->writeGo, &rw->mutex);
    }
    rw->waitingWriters -= 1;
    rw->activeWriters += 1;
    pthread_mutex_unlock(&rw->mutex);
}

void writer_unlock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->mutex);
    rw->activeWriters -= 1;
    if (rw->activeWriters == 0 && rw->waitingWriters > 0) {
        pthread_cond_signal(&rw->writeGo);

    } else {
        pthread_cond_broadcast(&rw->readGo);
    }
    pthread_mutex_unlock(&rw->mutex);
}

static int reader_should_wait(rwlock_t *rw) {
    if (rw->p == WRITERS) {
        if (rw->activeWriters > 0 || rw->waitingWriters > 0) {
            return 1;
        }
    } else if (rw->p == READERS) {
        if (rw->activeWriters > 0 || rw->waitingReaders > 0) {
            return 1;
        }
    }

    return 0;
}

static int writer_should_wait(rwlock_t *rw) {
    if (rw->p == WRITERS) {
        if (rw->activeWriters > 0 || rw->activeReaders > 0) {
            return 1;
        }
    } else if (rw->p == READERS) {
        if (rw->activeReaders > 0 || rw->waitingReaders > 0) {
            return 1;
        }
    }

    return 0;
}
