#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <inttypes.h>
#include <unistd.h>

#define OPTIONS "N:FLC"
#define F 1
#define L 2
#define C 3
int main(int argc, char *argv[]) {
    int size = 0;
    int policy = 0;
    int opt;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'N':
            size = atoi(optarg); // Convert the argument to an integer
            break;
        
        case 'F':
            policy = F;
            break;

        case 'L':
            policy = L;
            break;

        case 'C':
            policy = C;
            break;

        case '?':
            // Invalid option or missing argument
            fprintf(stderr, "Usage: %s [-t threads] <port>\n", argv[0]);
            return 1;
        }
    }

    


    return 0;
}


