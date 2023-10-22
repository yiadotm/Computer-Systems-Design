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

    Listener_Socket* sock;
    int fd = listener_init(sock, atoi(port));

    while (1) {
        int connection = listener_accept(sock);
        char buf[4096] = "";
        //check if httpserver can bind to the provided port
        int total = my_read(STDIN_FILENO, buf, 4096);
        // print_arr(buf, total);

    }



    


    return 0;
}
