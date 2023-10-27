#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "asgn2_helper_funcs.h"
#include "Request.h"
#define UNUSED(x) (void) x

int check_file(char *file, int cmd, int outfile);

void message_body(int code, int file);

int put_write(int infile, int outfile, Request* line);

int get_write(int infile, int outfile);

int set_write(int infile, int outfile, int content_length);

void print_arr(char buf[], int total);

ssize_t my_read(int in, char buf[], size_t nbytes);
