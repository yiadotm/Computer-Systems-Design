#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <regex.h>
#include <unistd.h>
#include "asgn2_helper_funcs.h"
#include "io.h"
#include "Request.h"

Request *newRequest(void) {
    Request *r = (Request *) malloc(sizeof(Request));
    r->version = 0.0;
    r->method = (char *) malloc(sizeof(char *));
    r->uri = (char *) malloc(sizeof(char *));
    r->content_length = 0;
    return r;
}
void freeRequest(Request *pR) {
    if (pR != NULL) {
        // free(pR->method);
        // free(pR->uri);
        free(pR);
    }
}
Request *build_request(char *header) {
    regex_t reg;
    char *re
        = "^([a-zA-Z]{1,8}) (/[a-zA-Z0-9.-]{1,63}) HTTP/([0-9].[0-9])\r\n([a-zA-Z0-9.-]{1,128}: "
          "[ -~]{1,128}\r\n)*\r\n$";

    int result = regcomp(&reg, re, REG_EXTENDED | REG_NEWLINE);
    result = regexec(&reg, header, 0, NULL, 0);
    if (result != 0) {
        // bad request
        // printf("here2\n");
        regfree(&reg);
        return NULL;
    }
    Request *line = newRequest();
    line->method = strtok(header, " ");
    // write_n_bytes(fd, line->method, 50);
    line->uri = strtok(NULL, " /");
    strtok(NULL, "/");
    line->version = atof(strtok(NULL, "\r\n"));
    char *str;
    while ((str = strtok(NULL, "\n"))) {
        char *last, *key;
        key = strtok_r(str, " ", &last);
        if (strcmp(key, "Content_Length:") == 0) {
            line->content_length = atoi(strtok_r(NULL, "", &last));
        }
    }
    regfree(&reg);
    return line;
}

void print_request(Request *r) {
    printf("Method: %s\n", r->method);
    printf("URI: %s\n", r->uri);
    printf("version: %0.1f\n", r->version);
    printf("\n");
}
