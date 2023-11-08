#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>

typedef struct queue queue_t;
typedef void *QueueElement;
typedef struct queue {
    int size;
    QueueElement *buffer;
} queue_t;

int in, out, count = 0;

queue_t *queue_new(int size) {
    queue_t *q = (queue_t *) malloc(sizeof(queue_t *));
    if (q == NULL) {
        return NULL;
    }
    q->size = size;
    q->buffer = (QueueElement *) malloc(size * sizeof(QueueElement));
    if (q->buffer == NULL) {
        free(q);
        return NULL; // Allocation for data failed
    }
    return q;
}
void queue_delete(queue_t **q) {
    if (q != NULL) {
        free((*q)->buffer);
        free(*q);
        *q = NULL;
    }
}

bool queue_push(queue_t *q, void *elem) {
    if (q == NULL) {
        return false;
    }

    q->buffer[in] = elem;
    in = (in + 1) % q->size;
    count += 1;
    return true;
}
bool queue_pop(queue_t *q, void **elem) {
    if (q == NULL) {
        return false;
    }

    *elem = q->buffer[out];
    out = (out + 1) % q->size;
    count -= 1;
    return true;
}
