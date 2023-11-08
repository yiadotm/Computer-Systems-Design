#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>
#include <semaphore.h>
#include <assert.h>

typedef struct queue queue_t;
typedef void *QueueElement;
typedef struct queue {
    int size;
    QueueElement *buffer;
    sem_t empty;
    sem_t full;
    sem_t mutex;
} queue_t;

int in, out, count = 0;

queue_t *queue_new(int size) {
    queue_t *q = (queue_t *) malloc(sizeof(queue_t));
    if (q == NULL) {
        return NULL;
    }
    q->size = size;
    q->buffer = (QueueElement *) malloc(size * sizeof(QueueElement));
    int rc = 0;
    rc = sem_init(&q->empty, 0, q->size);
    rc = sem_init(&q->full, 0, 0);
    rc = sem_init(&q->mutex, 0, 1);
    assert(!rc);
    if (q->buffer == NULL) {
        free(q);
        return NULL; // Allocation for data failed
    }
    return q;
}
void queue_delete(queue_t **q) {
    if (*q != NULL) {
        int rc = 0;
        rc = sem_destroy(&(*q)->empty);
        rc = sem_destroy(&(*q)->full);
        rc = sem_destroy(&(*q)->mutex);
        assert(!rc);
        free((*q)->buffer);
        free(*q);
        *q = NULL;
    }
}

bool queue_push(queue_t *q, void *elem) {
    if (q == NULL) {
        return false;
    }

    sem_wait(&q->empty);
    sem_wait(&q->mutex);
    q->buffer[in] = elem;
    in = (in + 1) % q->size;
    // count += 1;
    sem_post(&q->mutex);
    sem_post(&q->full);
    return true;
}
bool queue_pop(queue_t *q, void **elem) {
    if (q == NULL) {
        return false;
    }

    sem_wait(&q->full);
    sem_wait(&q->mutex);
    *elem = q->buffer[out];
    out = (out + 1) % q->size;
    // count -= 1;
    sem_post(&q->mutex);
    sem_post(&q->empty);

    return true;
}
