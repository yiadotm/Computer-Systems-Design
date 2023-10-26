#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <regex.h>
#include <unistd.h>
#include "asgn2_helper_funcs.h"
#include "io.h"

int set_write(int infile, int outfile, int content_length) {
    int length = 512;
    if (content_length < 512) {
        length = content_length;
    }
    //write the contents of location to STDOUT
    uint8_t *buff[length];
    int count = 0;
    ssize_t b_read = 0, b_write = 0;
    do {
        b_read = read(infile, buff, length);
        if (b_read == -1) {
            fprintf(stderr, "Invalid Command\n");
            close(infile);
            exit(1);
        }
        b_write = 0;
        do {
            size_t bytes = write(outfile, buff + b_write, b_read);
            if (bytes < 0) {
                fprintf(stderr, "Invalid Command\n");
                close(infile);
                exit(1);
            }
            b_write += bytes;
            count += bytes;
        } while (b_write < b_read && count < content_length);
    } while (count < content_length && b_read > 0 && b_write >= 0);

    close(infile);
    return 0;
}

int get_write(int infile, int outfile, int bytes) {
    //write the contents of location to STDOUT
    uint8_t *buff[bytes];
    ssize_t b_read = 0, b_write = 0;
    do {
        b_read = read(infile, buff, bytes);
        if (b_read == -1) {
            fprintf(stderr, "Invalid Command\n");
            close(infile);
            exit(1);
        }
        b_write = 0;
        do {
            size_t bytes = write(outfile, buff + b_write, b_read);
            if (bytes < 0) {
                fprintf(stderr, "Invalid Command\n");
                close(infile);
                exit(1);
            }
            b_write += bytes;
        } while (b_write < b_read);
    } while (b_read > 0 && b_write >= 0);

    close(infile);
    return 0;
}

void print_arr(char buf[], int total) {
    for (int i = 0; i < total; i++) {
        printf("%c", buf[i]);
    }
}

ssize_t my_read(int in, char buf[], size_t nbytes) {
    size_t bytes_read = 0;
    ssize_t total = 0;
    char c;

    // int countEndChar = 0;
    do {
        bytes_read = read(in, &c, 1);
        //error is encountered
        if (bytes_read == (size_t) -1) {
            return -1;
        }
        //if in is closed
        if (bytes_read == 0) {
            return total;
        }
        buf[total++] = c;



        if (total >= 4 && strcmp(buf + total - 4, "\r\n\r\n") == 0) {
            // printf("done\n");
            return total;
        }


    } while (bytes_read <= nbytes);
    return total;
}
