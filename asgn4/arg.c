#include <stdio.h>
#include <stdlib.h>
#include "arg.h"

Arguments *newArguments(queue_t *queue, FileLock *file) {
    Arguments *a = (Arguments *) malloc(sizeof(Arguments));
    a->q = queue;
    a->fi = file;
    return a;
}

void freeArguments(Arguments **pA) {
    if (*pA != NULL) {
        free((*pA)->fi);
        free(*pA);
        *pA = NULL;
    }
}
