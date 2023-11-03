#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct queue queue_t;

typedef struct queue_node {
    void *data;
    struct queue_node *next;
} queue_node_t;

typedef struct queue {
    int size;
    queue_node_t *front;
    queue_node_t *back;

} queue_t;

queue_t *queue_new(int size) {
    queue_t *q = (queue_t *)malloc(sizeof(queue_t));
    if (q == NULL) {
        return NULL;
    }
    q->size = size;
    q->front = q->back = NULL;
    return q;
}

void queue_delete(queue_t **q) {
    if (q != NULL && *q != NULL) {
        while ((*q)->front != NULL) {
            queue_node_t *temp = (*q)->front;
            (*q)->front = (*q)->front->next;
            free(temp);
        }
        free(*q);
        *q = NULL;
    }
}

bool queue_push(queue_t *q, void *elem) {
    if (q == NULL) {
        return false;
    }

    queue_node_t *new_node = (queue_node_t *)malloc(sizeof(queue_node_t));
    if (new_node == NULL) {
        return false;
    }
    new_node->data = elem;
    new_node->next = NULL;

    if (q->back == NULL) {
        q->front = q->back = new_node;
    }
    else {
        q->back->next = new_node;
        q->back = new_node;
    }
    return true;
}


bool queue_pop(queue_t *q, void **elem) {
    if (q == NULL || q->front == NULL || elem == NULL) {
        return false;
    }
    queue_node_t *temp = q->front;
    *elem = temp->data;
    q->front = q->front->next;
    
    free(temp);

    if (q->front == NULL) {
        q->back = NULL;
    }

    return true;
}
