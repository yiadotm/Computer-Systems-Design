#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <regex.h>

int set_write(int infile, int outfile, int content_length) {
    int length = 512;
    if (content_length < 512) {
        length = content_length;
    }
    //write the contents of location to STDOUT
    uint8_t *buff[length];
    int count = 0;
    ssize_t b_read = 0, b_write = 0;
    do {
        b_read = read(infile, buff, length);
        if (b_read == -1) {
            fprintf(stderr, "Invalid Command\n");
            close(infile);
            exit(1);
        }
        b_write = 0;
        do {
            size_t bytes = write(outfile, buff + b_write, b_read);
            if (bytes < 0) {
                fprintf(stderr, "Invalid Command\n");
                close(infile);
                exit(1);
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
            exit(1);
        }
        b_write = 0;
        do {
            size_t bytes = write(outfile, buff + b_write, b_read);
            if (bytes < 0) {
                fprintf(stderr, "Invalid Command\n");
                close(infile);
                exit(1);
            }
            b_write += bytes;
        } while (b_write < b_read);
    } while (b_read > 0 && b_write >= 0);

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
    uint8_t c;
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
    char *buffer = malloc(sizeof(char *) * 4097);
    size_t length = 0;
    int infile = STDIN_FILENO;
    int outfile = STDOUT_FILENO;

    read_line(1, buffer, infile, length);

    //add the input to the str array
    char *command;
    command = strtok(buffer, "\n");

    //check for "get" or "set" and if location is set
    if (strcmp(command, "get") != 0 && strcmp(command, "set") != 0) {
        fprintf(stderr, "Invalid Command\n");
        return 1;
    }

    //"get" option
    if (strcmp(command, "get") == 0) {
        //open the file to read
        length = 0;
        read_line(1, buffer, infile, length);

        if (!check_option(buffer, 0)) {
            fprintf(stderr, "Invalid Command\n");
            return 1;
        }
        char *location = strtok(buffer, "\n");
        //check the length of location
        int file = open(location, O_RDONLY);
        if (file == -1) {
            fprintf(stderr, "Invalid Command\n");
            close(file);
            return 1;
        }

        if (flush()) {
            fprintf(stderr, "Invalid Command\n");
            return 1;
        }
        get_write(file, outfile, 512);

        write(file, "\n", 1);
    }

    //"set" option

    if (strcmp(command, "set") == 0) {
        read_line(2, buffer, infile, length);
        if (!check_option(buffer, 1)) {
            fprintf(stderr, "Invalid Command1\n");
            return 1;
        }
        char *location = strtok(buffer, "\n");
        char *content_length = strtok(NULL, "\n");

        //check the length of location

        //open the file to write
        int file = open(location, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (file == -1) {
            fprintf(stderr, "Invalid Command3\n");
            return 1;
        }

        set_write(infile, file, atoi(content_length));
        //write "OK" at the end
        fprintf(stdout, "OK\n");
        close(file);
    }

    free(buffer);

    return 0;
}
