#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <error.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Note when running this exercise, cakelog will error on initialise because you run the executable twice
// at the same time. This means the timestamps will match and one process will create the file then another will
// attempt to create the same file. So we don't use it this time - just good old printf().

// Without the O_APPEND flag the file is half the size. Presumably because the lseek/write operation is not atomic so the
// processes essentially write over each other. With the O_APPEND flag, Linux makes sure the write is completed to the
// file before allowing any other process at it.

int main(int argc, char** argv) {

    char* filename = argv[1];
    int num_bytes = atol(argv[2]);

    int append_flag;
    if (argc == 4) {
        append_flag = 0;
    }
    else {
        append_flag = O_APPEND;
    }

    int fd = open(argv[1], append_flag | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1) {
        perror("open()");
        printf("Unable to open file [%s]\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    else {
        printf("Succesfully opened [%s] for writing\n", argv[1]);
    }

    char byte_to_write='X';
    ssize_t bytes_written;
    ssize_t total_bytes_written = 0;

    for (long i=0; i<num_bytes; i++) {
        if (append_flag) {
            int seekres = lseek(fd, 0, SEEK_END);
            if (seekres == -1) {
                printf("Problem seeking to location 0\n");
                perror("lseek()");
            }
        }
        bytes_written = write(fd, &byte_to_write, 1);

        if (bytes_written != 1) {
            printf("Problem writing byte [%ld] to the file\n", i);
            perror("write()");
        }

        total_bytes_written += bytes_written;
    }

    close(fd);
    printf("Written [%ld] bytes to [%s]\n", total_bytes_written, argv[1]);
    

}

