#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "asgn2_helper_funcs.h"
#define UNUSED(x) (void)x

typedef struct {
    char* method;
    char* uri;
    double version;
    int content_length;

} Request;

Request* newRequest(void);

void freeRequest(Request* pR);

Request *build_request(char *header);

void print_request(Request* r);


void print_arr(char buf[], int total);

ssize_t my_read(int in, char buf[], size_t nbytes);

