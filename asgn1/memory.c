#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <regex.h>

//function that counts elements in array
int len_arr(const char *word) {
    int length = 0;
    while (word[length] != '\0') {
        length++;
    }
    return length;
}
int get_write(int infile, int outfile, int bytes) {
    //write the contents of location to STDOUT
    //referenced code from ChatGPT
    char buff[bytes];
    ssize_t b_read = 0, b_write = 0;
    do {
        b_read = read(infile, buff, sizeof(buff));
        if (b_read == -1) {
            fprintf(stderr, "Invalid Command\n");
            close(infile);
            return 1;
        }
        b_write = 0;
        do {
            size_t bytes = write(outfile, buff + b_write, b_read);
            if (bytes < 0) {
                fprintf(stderr, "Invalid Command\n");
                close(infile);
                return 1;
            }
            b_write += bytes;
        } while (b_write < b_read);
    } while (b_read > 0 && b_write >= 0);

    // while ((b_read = read(infile, buff, sizeof(buff))) > 0) {
    //     ssize_t b_write = write(outfile, buff, b_read);
    //     if (b_write == -1) {
    //         fprintf(stderr, "Error writing to stdout\n");
    //         close(infile);
    //         return 1;
    //     }
    // }

    close(infile);
    return 0;
}
void read_line(int newlineCount, char *buffer, int infile, size_t length) {
    ssize_t bytes_read;
    char c;
    //read from STDIN
    int newline = 0;
    while ((bytes_read = read(infile, &c, 1)) > 0) {

        if (c == '\n') {
            newline++;
        }
        buffer[length++] = c;

        if (newline == newlineCount) {
            break;
        }
    }
}
int flush() {
    ssize_t bytes_read;
    char c;
    int flushed = 0;
    while ((bytes_read = read(STDIN_FILENO, &c, 1)) > 0) {
        if (bytes_read > 0) {
            flushed = 1;
        }
    }
    return flushed;
}
int check_option(char *header, int option) {
    regex_t preg; // make a regex object thing
    char *g = "^[ -~]{1,4096}\n$"; // this is the "format" that you're comparing header with
    char *s = "^[ -~]{1,4096}\n[0-9]{1,10}\n$";
    int result = 0;
    if (option == 0) {
        result = regcomp(&preg, g, REG_EXTENDED | REG_NEWLINE); // regex compile, initialize regex
    } else {
        result = regcomp(&preg, s, REG_EXTENDED | REG_NEWLINE); // regex compile, initialize regex
    }
    result = regexec(&preg, header, 0, NULL,
        0); // regex execute, compares header with the format and stores a value in result
    regfree(&preg);
    if (result) { // if result != 0, header doesnt have a correct format
        // bad request
        return 0;
    }
    return 1;
}

int main() {
    char *buffer = malloc(sizeof(char *) * 512);

    size_t length = 0;
    char c;

    int infile = STDIN_FILENO;
    int outfile = STDOUT_FILENO;

    read_line(1, buffer, infile, length);

    //add the input to the str array
    char **str = malloc(sizeof(char *) * length);
    char *command;
    command = strtok(buffer, "\n");

    //check for "get" or "set" and if location is set
    if (strcmp(command, "get") != 0 && strcmp(command, "set") != 0) {
        fprintf(stderr, "Invalid Command\n");
        return 1;
    }

    //"get" option
    if (strcmp(buffer, "get") == 0) {
        //open the file to read
        length = 0;
        read_line(1, buffer, infile, length);

        if (!check_option(buffer, 0)) {
            fprintf(stderr, "Invalid Command\n");
            return 1;
        }
        char *location = strtok(buffer, "\n");

        int file = open(location, O_RDONLY);
        if (file == -1) {
            fprintf(stderr, "Invalid Command\n");
            close(file);
            return 1;
        }
        //check for validity of location
        if (len_arr(location) > PATH_MAX) {
            fprintf(stderr, "Filename is greater than PATH_MAX\n");
            close(file);
            return 1;
        }
        // if (strchr(str[1], '\0')) {
        //     fprintf(stderr, "Filename includes NULL character!\n");
        //     return 1;
        // }
        if (flush()) {
            fprintf(stderr, "Invalid Command\n");
            return 1;
        }
        get_write(file, outfile, PATH_MAX);

        write(file, "\n", 1);
    }

    //"set" option

    if (strcmp(buffer, "set") == 0) {
        read_line(2, buffer, infile, length);
        if (!check_option(buffer, 1)) {
            fprintf(stderr, "Invalid Command\n");
            return 1;
        }
        char *location = strtok(buffer, "\n");
        char *content_length = strtok(NULL, "\n");
        //open the file to write
        int file = open(location, O_WRONLY | O_TRUNC, 0644);
        if (file == -1) {
            fprintf(stderr, "Invalid Command\n");
            return 1;
        }
        //closes stdin before providing content_length bytes
        // if (count < 3) {
        //     //write all the contents to STDOUT
        // }
        // char buff[PATH_MAX];
        ssize_t b_read;
        int num = 0;
        while ((b_read = read(infile, &c, 1)) > 0) {
            ssize_t b_write = 0;
            if (num < atoi(content_length)) {
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
    for (int i = 0; i < (int) length; i++) {
        free(str[i]);
    }
    free(str);
    //read the inputs separated by '\n'
    //check if first input is 'get' or 'set' (case-sensitive)
    //check for invalid commands

    return 0;
}
