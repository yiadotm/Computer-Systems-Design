#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <inttypes.h>
#include <unistd.h>
#include "cache.h"

#define OPTIONS "N:FLC"

int main(int argc, char *argv[]) {
    int size = 0;
    int policy = 1;
    int opt;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'N':
            size = atoi(optarg); // Convert the argument to an integer
            break;

        case 'F': policy = F; break;

        case 'L': policy = L; break;

        case 'C': policy = C; break;

        case '?':
            // Invalid option or missing argument
            fprintf(stderr, "Usage: %s [-N size] <policy>\n", argv[0]);
            return 1;
        }
    }
    // // Check if a valid policy is provided
    // if (policy == INVALID_POLICY) {
    //     fprintf(stderr, "Error: Policy not specified (use -F, -L, or -C)\n");
    //     return 1;
    // }
    if (size == 0) {
        fprintf(stderr, "Error: size not specified (-N size)\n");
        return 1;
    }
    // printf("size: %d\npolicy: %d\n", size, policy);

    struct Cache cache;
    initializeCache(&cache, size, policy);

    char buffer[1024];
    int compulsoryMisses = 0;
    int capacityMisses = 0;
    // Read from stdin until EOF
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character
        handleCacheAccess(&cache, buffer);
    }
    // Calculate misses
    compulsoryMisses = cache.CO;
    capacityMisses = cache.CA;
    // Print summary
    printf("%d %d\n", compulsoryMisses, capacityMisses);

    freeCache(&cache);
    return 0;
}
