#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>

//function that counts elements in array
int len_arr(const char* word[]) {
    int length = 0;
    while (word[length] != NULL) {
        length++;
    }
    return length;
}

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
    int count = 0;
    while (pch != NULL) {
        str[count] = pch;
        count++;
        if (count == 4) {
            break;
        }
        pch = strtok(NULL, "\n");

    }
    // printf("count: %d\n", count);
    // for (int i = 0; i < count; i++) {
    //     printf("%s\n", str[i]);
    // }
    //check for "get" or "set" and if location is set
    if (strcmp(str[0], "get") != 0 || strcmp(str[0], "set") != 0 || count <= 1) {
        fprintf(stderr, "Invalid Command!\n");
        return 1;
    }

    //"get" option
    if (strcmp(str[0], "get") == 0) {

        //check for validity of location
        if (len_arr(str(1)) > PATH_MAX) {
            fprintf(stderr, "Filename is greater than PATH_MAX!\n");
            return 1;            
        }
        if (strchr(str[1], "\0")) {
            fprintf(stderr, "Filename includes NULL character!\n");
            return 1;              
        }

        //write the contents of location to STDOUT
    }

    //"set" option

    if (strcmp(str[0], "set") == 0) {
        
    }


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
