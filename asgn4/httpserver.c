#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include "asgn2_helper_funcs.h"
#include "queue.h"
#include "request.h"
#include "response.h"
#include "rwlock.h"
#include "connection.h"

#define OPTIONS "t:"

int main(int argc, char *argv[]) {
    //chatGPT
    int threads = 1; // Default number of threads

    // Process command-line options
    int opt;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 't':
            threads = atoi(optarg); // Convert the argument to an integer
            break;
        case '?':
            // Invalid option or missing argument
            fprintf(stderr, "Usage: %s [-t threads] <port>\n", argv[0]);
            return 1;
        }
    }

    // Process the remaining non-option arguments (the port)
    if (optind < argc) {
        int port = atoi(argv[optind]);
        // Now you can use 'threads' and 'port' in your program
        printf("Threads: %d\n", threads);
        printf("Port: %d\n", port);
    } else {
        // Missing required arguments
        fprintf(stderr, "Usage: %s [-t threads] <port>\n", argv[0]);
        return 1;
    }




    return 0;
}
