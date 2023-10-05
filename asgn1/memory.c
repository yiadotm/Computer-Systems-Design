#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>

//function that either reads or writes the bytes
int bytes_funct(int file, uint8_t *buf, int nbytes, int opt) {
    ssize_t curr_num = 0; // number of bytes from the most recent read/write
    int total = 0; // total number of bytes that have been read/written
    while (total < nbytes) {
        //opt == 1 means read, 0 means write
        if (opt == 1) {
            curr_num = read(file, buf, nbytes - total);
        } else {
            curr_num = write(file, buf, nbytes - total);
        }

        if ((int) curr_num <= 0) {
            break;
        }
        total += (int) curr_num;
    }
    return total;
}

//function that reads the bytes, uses the bytes_funct
int read_bytes(int infile, uint8_t *buf, int to_read) {
    return bytes_funct(infile, buf, to_read, 1);
}

//function that writes the bytes, uses the bytes_funct
int write_bytes(int outfile, uint8_t *buf, int to_write) {
    return bytes_funct(outfile, buf, to_write, 0);
}

int main(int argc, char *argv[]) {
    int infile = STDIN_FILENO;
    int outfile = STDOUT_FILENO;

    infile = open(infile, O_RDONLY);
    //Open infile with open
    //Print message if error
    if (infile == -1) {
        fprintf(stderr, "Error opening input file\n");
        exit(1);
    }

    return 0;
}