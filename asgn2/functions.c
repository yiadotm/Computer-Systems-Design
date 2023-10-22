#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <regex.h>
#include <unistd.h>
#include "asgn2_helper_funcs.h"

typedef struct {
    char* method;
    char* uri;
    double version;
    int content_length;

} Request;

Request *build_request(char *header, int in_fd) {
    regex_t reg; // make a regex object thing
    char *re = "^([a-zA-Z]{1,8}) (/[a-zA-Z0-9.-]{1,63}) HTTP/([0-9]\\.[0-9])\r\n([a-zA-Z0-9.-]{1,128}: "
          "[\x20-\x7E]{1,128}\r\n)*\r\n$"; // this is the "format" that you're comparing header with
    int result = regcomp(&reg, re, REG_EXTENDED | REG_NEWLINE); // regex compile, initialize regex
    result = regexec(&reg, header, 0, NULL, 0); // regex execute, compares header with the format and stores a value in result
    if (result) { // if result != 0, header doesnt have a correct format
        // bad request
        regfree(&reg);
        return NULL;
    }
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
    char endChar[10] = "";
    int countEndChar = 0;
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
        total += bytes_read;

        // //check if "\r\n\r\n" was read
        // if (c == '\r' || c == '\n') {
        //     endChar[countEndChar++] = c;
        // }

        // if (strncmp(endChar, "\r\n\r\n", 4) == 0) {
        //     printf("done\n");
        //     return total;
        // }
        //check if "\r\n\r\n" was read
        if (c == '\n') {
            endChar[countEndChar++] = c;
        }

        if (strncmp(endChar, "\n\n", 4) == 0) {
            return total;
        }

    } while (bytes_read <= nbytes);
    return total;
}
