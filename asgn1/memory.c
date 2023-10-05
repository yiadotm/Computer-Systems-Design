#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>


//get function
int main() {
    char* buffer = NULL;
    size_t buffer_size = 0;
    size_t length = 0;
    char c;
    ssize_t bytes_read;
    int infile = STDIN_FILENO;
    // int outfile = STDOUT_FILENO;
    
    while ((bytes_read = read(infile, &c, 1)) > 0) {
        if (length >= buffer_size) {
            // Double the buffer size if needed
            //code is referenced from ChatGPT
            buffer_size = (buffer_size == 0) ? 1 : buffer_size * 2;
            buffer = realloc(buffer, buffer_size);
            if (buffer == NULL) {
                fprintf(stderr, "Memory allocation failed");
                return 1;
            }
        }
        buffer[length++] = c;        
    }
    
    //add the input to the str array
    char** str = malloc(sizeof(char*)* 10);
    char* pch;
    pch = strtok(buffer, "\n");
    printf("%s\n", pch);
    int count = 0;
    while (pch != NULL) {
        str[count] = pch;
        count++;
        if (count == 3) {
            break;
        }
        pch = strtok(NULL, "\n");

    }
    printf("\n");
    for (int i = 0; i < count; i++) {
        printf("%s\n", str[i]);
    }
        // //check for "get" or "set"
        // if (strcmp(pch, "get") != 0 && strcmp(pch, "set") != 0) {
        //     fprintf(stderr, "Invalid Command~\n");
        //     return 1;
        // }


    // if (length > 0) {
    //     buffer[length] = '\0'; // Null-terminate the string
    //     printf("You entered: %s\n", buffer);
    // } else {
    //     printf("No input provided.\n");
    // }

    free(buffer);
    //read the inputs separated by '\n' 
    //check if first input is 'get' or 'set' (case-sensitive)
        //check for invalid commands

    return 0;
}
