#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Invalid Port\n");
        return 1;
    }

    //check if port passed to httpserver is invalid (not an integer between 1 and 65535)

    //check if httpserver can bind to the provided port
}