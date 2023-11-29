#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include "arg.h"

Arguments *newArguments(queue_t *queue, FileLock *file) {
    Arguments *a = (Arguments *) malloc(sizeof(Arguments));
    int rc = pthread_mutex_init(&a->mutex, NULL);
    assert(!rc);
    a->q = queue;
    a->fi = file;
    return a;
}

void freeArguments(Arguments **pA) {
    if (*pA != NULL) {
        int rc = pthread_mutex_destroy(&(*pA)->mutex);
        assert(!rc);
        free((*pA)->fi);
        free(*pA);
        *pA = NULL;
    }
}
