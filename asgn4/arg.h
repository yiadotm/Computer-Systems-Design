#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "asgn2_helper_funcs.h"
#include "queue.h"
#include "queue.h"
#include "request.h"
#include "response.h"
#include "debug.h"
#include "rwlock.h"
#include "connection.h"
#include "arg.h"
#include "file_lock.h"

typedef struct {
    queue_t *q;
    FileLock *fi;
} Arguments;

Arguments *newArguments(queue_t *queue, FileLock *file);

void freeArguments(Arguments **pA);
