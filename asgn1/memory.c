#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

//function that counts elements in array
int len_arr(const char *word) {
    int length = 0;
    while (word[length] != '\0') {
        length++;
    }
    return length;
}
int my_write(int infile, int outfile, int bytes) {
    //write the contents of location to STDOUT
    //referenced code from ChatGPT
    char buff[bytes];
    ssize_t b_read;
    while ((b_read = read(infile, buff, sizeof(buff))) > 0) {
        ssize_t b_write = write(outfile, buff, b_read);
        if (b_write == -1) {
            fprintf(stderr, "Error writing to stdout\n");
            close(infile);
            return 1;
        }
    }

    if (b_read == -1) {
        fprintf(stderr, "Error reading from file\n");
        close(infile);
        return 1;
    }
    close(infile);
    return 0;
}

int main() {
    char *buffer = NULL;
    size_t buffer_size = 0;
    size_t length = 0;
    char c;
    ssize_t bytes_read;
    int infile = STDIN_FILENO;
    int outfile = STDOUT_FILENO;

    //read from STDIN
    int newline = 0;
    while ((bytes_read = read(infile, &c, 1)) > 0) {
        if (length >= buffer_size) {
            // Double the buffer size if needed
            //code is referenced from ChatGPT
            buffer_size = (buffer_size == 0) ? 1 : buffer_size * 2;
            buffer = realloc(buffer, buffer_size);
            if (buffer == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                return 1;
            }
        }
        if (c == '\n') {
            newline++;
        }
        buffer[length++] = c;
        if (newline == 3) {
            break;
        }
    }

    //add the input to the str array
    char **str = malloc(sizeof(char *) * 10);
    char *pch;
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
    if (strcmp(str[0], "get") != 0 && strcmp(str[0], "set") != 0) {
        fprintf(stderr, "Invalid Command\n");
        return 1;
    } else if (count <= 1) {
        fprintf(stderr, "Invalid Command\n");
        return 1;
    }

    //"get" option
    if (strcmp(str[0], "get") == 0) {
        //open the file to read
        int file = open(str[1], O_RDONLY);
        if (file == -1) {
            fprintf(stderr, "Error opening file\n");
            close(file);
            return 1;
        }
        //check for validity of location
        if (len_arr(str[1]) > PATH_MAX) {
            fprintf(stderr, "Filename is greater than PATH_MAX\n");
            close(file);
            return 1;
        }
        // if (strchr(str[1], '\0')) {
        //     fprintf(stderr, "Filename includes NULL character!\n");
        //     return 1;
        // }

        my_write(file, outfile, PATH_MAX);
        // write(file, "\n", 1);
    }

    //"set" option

    if (strcmp(str[0], "set") == 0) {
        //open the file to write
        int file = open(str[1], O_WRONLY | O_TRUNC, 666);
        if (file == -1) {
            fprintf(stderr, "Error opening file\n");
            return 1;
        }
        //closes stdin before providing content_length bytes
        if (count < 3) {
            //write all the contents to STDOUT
        }
        // char buff[PATH_MAX];
        ssize_t b_read;
        int num = 0;
        while ((b_read = read(infile, &c, 1)) > 0) {
            ssize_t b_write = 0!;
            if (num < atoi(str[2])) {
                b_write = write(file, &c, b_read);
                num++;
            }

            if (b_write == -1) {
                fprintf(stderr, "Error writing to stdout\n");
                close(infile);
                return 1;
            }
        }
        //write "OK" at the end
        fprintf(stdout, "OK\n");
        close(file);
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
