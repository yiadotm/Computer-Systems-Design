#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <asgn2_helper_funcs.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Invalid Port\n");
        return 1;
    }

    //check if port passed to httpserver is invalid (not an integer between 1 and 65535)
    char* port = 0;
    read(STDIN_FILENO, port, 1);
    if (atoi(port) < 1 || atoi(port) > 65535) {
        fprintf(stderr, "Invalid Port\n");
        return 1;        
    }
    
    Listener_Socket* sock;
    listener_init(sock, atoi(port));

    //check if httpserver can bind to the provided port

    return 0;
}