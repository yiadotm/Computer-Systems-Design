#include <stdio.h>
#include <stdlib.h>
#include "asgn2_helper_funcs.h"
#include "queue.h"

typedef struct {
    Listener_Socket *sock;
    int threads;
    queue_t *q;
} Arguments;

Arguments *newArguments(Listener_Socket sock, int threads, queue_t *queue);

void freeArguments(Arguments **pA);
