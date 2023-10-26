#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "asgn2_helper_funcs.h"
#include "io.h"
#define UNUSED(x) (void) x

typedef struct {
    char *method;
    char *uri;
    char *version;
    int content_length;

} Request;

Request *newRequest(void);

void freeRequest(Request *pR);

Request *build_request(char *header, int fd);

void print_request(Request *r);
