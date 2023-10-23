#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "asgn2_helper_funcs.h"
#include "functions.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Invalid Port\n");
        return 1;
    }

    //check if port passed to httpserver is invalid (not an integer between 1 and 65535)
    char *port = argv[1];

    if (atoi(port) < 1 || atoi(port) > 65535) {
        fprintf(stderr, "Invalid Port\n");
        return 1;
    }
    Listener_Socket sock;
    int listen_fd = listener_init(&sock, atoi(port));

    if (listen_fd == -1) {
        printf("init failed\n");
    }

    while (1) {
        int con_fd = listener_accept(&sock);
        if (con_fd > 0) {
            char buf[4096] = "";
            //check if httpserver can bind to the provided port
            int total = my_read(con_fd, buf, 4096);

            write_n_bytes(STDOUT_FILENO, buf, total);
            // Request *line = build_request(buf);
            // UNUSED(line);
            // print_request(line);
            // if (strncmp(line->method, "GET", 8) == 0) {
            //     int file = open(line->uri, O_RDONLY);
            // }

            // if (strncmp(line->method, "PUT", 8) == 0) {

            // }
        }
        close(con_fd);
    }

    // char buf[4096] = "";
    // my_read(STDIN_FILENO, buf, 4096);
    // print_arr(buf, total);
    // Request *line = build_request(buf);
    // if (line == NULL) {
    //     fprintf(stderr, "request not valid.\n");
    //     return 1;
    // }
    // printf("pog\n");
    // print_request(line);
    // printf("here2\n");
    // freeRequest(line);
    return 0;
}
