#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
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
                message_body(500, con_fd);
            }
            // write_n_bytes(STDOUT_FILENO, buf, total);
            Request *line = build_request(buf);
            if (line == NULL) {
                message_body(400, con_fd);
            } else if (line->version != 1.1) {
                message_body(505, con_fd);
            } else if (strncmp(line->method, "GET", 8) == 0) {
                if (check_file(line->uri, 0, con_fd)) {
                    // print_request(line);
                    int file = open(line->uri, O_RDONLY);
                    if (file == -1) {
                        message_body(404, con_fd);
                    } else {

                        message_body(200, con_fd); //use lseek to find content length
                        struct stat file_info;
                        stat(line->uri, &file_info);
                        off_t file_size = file_info.st_size;
                        char size_string[20];
                        sprintf(size_string, "%lld", (long long) file_size);
                        write_n_bytes(con_fd, "Content-Length: ", 16);
                        write_n_bytes(con_fd, size_string, strlen(size_string));
                        write_n_bytes(con_fd, "\r\n\r\n", 4);
                        // int count = get_file_count(file);
                        // printf("%d\n", count);
                        get_write(file, con_fd);
                    }
                    close(file);
                }

            }

            else if (strncmp(line->method, "PUT", 8) == 0) {
                // if (check_file(line->uri, 1, con_fd)) {
                //     int file = open(line->uri, O_WRONLY | O_TRUNC, 0666);
                //     if (file == -1) {
                //         message_body(201, con_fd);
                //         file = open(line->uri, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                //     } else {
                //         message_body(200, con_fd);
                //         write_n_bytes(con_fd, "Content-Length: 3\r\n\r\nOK\n", 24);
                //     }
                //     int pass = 0;
                //     do {
                //         pass = pass_n_bytes(con_fd, file, 1);
                //     } while (pass > 0);

                //     close(file);
                // }
                int file = open(line->uri, O_WRONLY | O_TRUNC, 0644);
                if (access(line->uri, F_OK) < 0) {
                    message_body(201, con_fd);
                    file = open(line->uri, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                } else {
                    message_body(200, con_fd);
                    write_n_bytes(con_fd, "Content-Length: 3\r\n\r\nOK\n", 24);
                }
                int pass = 0;
                do {
                    pass = pass_n_bytes(con_fd, file, 1);
                } while (pass > 0);
                write_n_bytes(con_fd, "\n", 1);
                close(file);

            } else {
                message_body(501, con_fd);
            }

            freeRequest(line);
        }

        close(con_fd);
    }

    return 0;
}
