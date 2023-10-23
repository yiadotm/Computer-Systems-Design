#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <regex.h>
#include <unistd.h>
#include "asgn2_helper_funcs.h"

typedef struct {
    char *method;
    char *uri;
    double version;

} Request;

Request *newRequest(void) {
    Request *r = (Request *) malloc(sizeof(Request));
    r->version = 0.0;
    r->method = (char *) malloc(sizeof(char *));
    r->uri = (char *) malloc(sizeof(char *));
    return r;
}
void freeRequest(Request *pR) {
    if (pR != NULL) {
        free(pR->method);
        free(pR->uri);
    }
}

Request *build_request(char *header) {

    regex_t reg; // make a regex object thing
    char *re
        = "^([a-zA-Z]{1,8}) (//[a-zA-Z0-9.-]{1,63}) "
          "HTTP//([0-9]\\.[0-9])\r\n([a-zA-Z0-9.-]{1,128}: "
          "[\x20-\x7E]{1,128}\r\n)*\r\n$"; // this is the "format" that you're comparing header with
    int result = regcomp(&reg, re, REG_EXTENDED | REG_NEWLINE); // regex compile, initialize regex
    result = regexec(&reg, header, 0, NULL,
        0); // regex execute, compares header with the format and stores a value in result
    printf("here1\n");
    if (result) { // if result != 0, header doesnt have a correct format
        // bad request
        printf("here2\n");
        regfree(&reg);
        return NULL;
    }
    Request *line = newRequest();
    line->method = strtok(header, " ");
    printf("Method: %s\n", line->method);
    strtok(header, "/");
    line->uri = strtok(header, " ");
    printf("URI: %s\n", line->uri);
    strtok(header, "HTTP/");
    line->version = atoi(strtok(header, "\n"));
    printf("version: %f\n", line->version);
    return line;
}
void print_request(Request *r) {
    printf("Method: %s\n", r->method);
    printf("URI: %s\n", r->uri);
    printf("version: %f\n", r->version);
    printf("\n");
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

        // //check if "\r\n\r\n" was read
        // if (c == '\r' || c == '\n') {
        //     endChar[countEndChar++] = c;
        // }

        if (total >= 4 && strcmp(buf + total - 3, "\r\n\r\n") == 0) {
            printf("done\n");
            return total;
        }

        // //check if "\r\n\r\n" was read
        // if (c == '\n') {
        //     countEndChar++;
        // } else {
        //     countEndChar = 0;
        // }

        // if (countEndChar == 2) {
        //     return total;
        // }

    } while (bytes_read <= nbytes);
    return total;
}
