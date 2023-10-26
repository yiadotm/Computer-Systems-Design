#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "asgn2_helper_funcs.h"
#include "io.h"
#include "Request.h"

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
            if (total == -1) {
                fprintf(stderr, "my_read failed\n");
            }
            // write_n_bytes(STDOUT_FILENO, buf, total);
            Request *line = build_request(buf, con_fd);
            if (line == NULL) {
                message_body(400, con_fd);
            }

            print_request(line);
            if (strncmp(line->method, "GET", 8) == 0) {
                int file = open(line->uri, O_RDONLY);
                if (file == -1) {
                    message_body(404, con_fd);
                } else {
                    message_body(200, con_fd);
                    pass_n_bytes(file, con_fd, line->content_length);
                }
            }

            else if (strncmp(line->method, "PUT", 8) == 0) {
                int file = open(line->uri, O_WRONLY | O_TRUNC, 0666);
                if (file == -1) {
                    message_body(201, con_fd);
                    file = open(line->uri, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                } else {
                    message_body(200, con_fd);
                }
                pass_n_bytes(con_fd, file, line->content_length);
            } else {
                message_body(501, con_fd);
            }
        }
        close(con_fd);
    }

    return 0;
}
