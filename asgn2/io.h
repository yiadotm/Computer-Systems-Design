#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "asgn2_helper_funcs.h"
#include "Request.h"
#define UNUSED(x) (void) x

void message_body(int code, int file);

int set_write(int infile, int outfile, int content_length);

int get_write(int infile, int outfile, int bytes);

void print_arr(char buf[], int total);

ssize_t my_read(int in, char buf[], size_t nbytes);
