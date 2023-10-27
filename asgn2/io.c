#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <regex.h>
#include <unistd.h>
#include "asgn2_helper_funcs.h"
#include "io.h"
#include "Request.h"

int check_file(char *file, int cmd, int outfile) {
    if (cmd == 0) { //get
        if (access(file, F_OK) < 0) {
            message_body(404, outfile);
            return 1;
        } else if (access(file, R_OK) < 0) {
            message_body(403, outfile);
            return 1;
        }

    } else { //set
        if (access(file, F_OK) < 0) {
            message_body(201, outfile);
            return 1;
        } else if (access(file, R_OK) < 0) {
            message_body(403, outfile);
            return 1;
        }
    }
    return 0;
}

void message_body(int code, int file) {

    if (code == 200) { // When a method is Successful
        write_n_bytes(file, "HTTP/1.1 200 OK\r\n", 17);
        return;
    } else if (code == 201) { // When a URI's file is created
        write_n_bytes(file, "HTTP/1.1 201 Created\r\nContent-Length: 8\r\n\r\nCreated\n", 51);
        return;
    } else if (code == 400) { // When a request is ill-formatted
        write_n_bytes(
            file, "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad Request\n", 60);
        return;
    } else if (code == 403) { // When a URI's file is not accessible
        write_n_bytes(file, "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\nForbidden\n", 56);
        return;
    } else if (code == 404) { // WHen the URI's file does not exist
        write_n_bytes(file, "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\nNot Found\n", 56);
        return;
    } else if (code == 500) { // When an unexpected issue prevents processing
        write_n_bytes(file,
            "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 22\r\n\r\nInternal Server "
            "Error\n",
            80);
        return;

    } else if (code == 501) { // When a request includes an unimplemented Method
        write_n_bytes(file,
            "HTTP/1.1 501 Not Implemented\r\nContent-Length: 16\r\n\r\nNot Implemented\n", 68);
        return;

    } else if (code == 505) { // When a request includes an unsupported version
        write_n_bytes(file,
            "HTTP/1.1 505 Version Not Supported\r\nContent-Length: 22\r\n\r\nVersion Not "
            "Supported\n",
            80);
        return;
    }
}
int get_write(int infile, int outfile) {
    //write the contents of location to STDOUT
    ssize_t b_read = 0;
    do {
        b_read = pass_n_bytes(infile, outfile, 4096);
        if (b_read == -1) {
            message_body(403, outfile);
            close(infile);
            return 1;
        }

    } while (b_read > 0);

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
