#include <stdio.h>
#include <stdlib.h>
#include "arg.h"

Arguments *newArguments(Listener_Socket sock, int threads, queue_t *queue) {
    Arguments *a = (Arguments *) malloc(sizeof(Arguments));
    a->sock = (Listener_Socket *) malloc(sizeof(Listener_Socket));
    // a->q = (queue_t *) malloc(sizeof(queue_t *));
    a->q = queue;
    a->sock->fd = sock.fd;
    a->threads = threads;
    return a;
}

void freeArguments(Arguments **pA) {
    if ((*pA) != NULL && pA != NULL) {
        free((*pA)->sock);
        free((*pA)->q);
        free((*pA));
        *pA = NULL;
    }
}
