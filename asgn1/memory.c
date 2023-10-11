#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <regex.h>

//stackoverflow: https://stackoverflow.com/questions/4553012/checking-if-a-file-is-a-directory-or-just-a-file
int is_regular_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

//function that counts elements in array
int len_arr(const char *word) {
    int length = 0;
    while (word[length] != '\0') {
        length++;
    }
    return length;
}
int set_write(int infile, int outfile, int content_length) {
    //write the contents of location to STDOUT
    char buff[512];
    ssize_t count = 0;
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
            count += bytes;
        } while (b_write < b_read && count < content_length);
    } while (count < content_length && b_read > 0 && b_write >= 0);

    close(infile);
    return 0;
}

int get_write(int infile, int outfile, int bytes) {
    //write the contents of location to STDOUT
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
//referenced ChatGPT
int check_option(char *header, int option) {
    // make a regex object
    regex_t preg;
    //format comparison
    char *g = "^[ -~]{1,4096}\n$";
    char *s = "^[ -~]{1,4096}\n[0-9]{1,10}\n$";
    int result = 0;
    if (option == 0) {
        //get pattern
        result = regcomp(&preg, g, REG_EXTENDED | REG_NEWLINE);
    } else {
        //set pattern
        result = regcomp(&preg, s, REG_EXTENDED | REG_NEWLINE);
    }
    // regex execute, compares string with the pattern and stores a value in result
    result = regexec(&preg, header, 0, NULL, 0);
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
    // char c;

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
        if (!is_regular_file(location)) {
            fprintf(stderr, "Invalid Command\n");
            return 1;
        }
        int file = open(location, O_RDONLY);
        if (file == -1) {
            fprintf(stderr, "Invalid Command\n");
            close(file);
            return 1;
        }
        //check for validity of location
        if (len_arr(location) > PATH_MAX) {
            fprintf(stderr, "Filename is greater than PATH_MAX.\n");
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
        if (!is_regular_file(location)) {
            fprintf(stderr, "Invalid Command\n");
            return 1;
        }
        //open the file to write
        int file = open(location, O_WRONLY | O_TRUNC, 0644);
        if (file == -1) {
            fprintf(stderr, "Invalid Command\n");
            return 1;
        }
        set_write(infile, file, atoi(content_length));
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

    return 0;
}
