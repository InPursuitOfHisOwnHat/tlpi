#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include "cakelog/cakelog.h"

// The answer to this exercise is that if you open a file with O_APPEND, calls to write() will *always* append, even if you seek
// to a different location before you write.

int main(int argc, char** argv) {

    if (cakelog_initialise("append-seek") != 0) {
        printf("main() : error initialising logging. Fast fail.\n");
        exit(EXIT_FAILURE);
    }

    if ( argc < 4) {
        cakelog("Incorrect number of parameters passed ([%d]), terminating with EXIT_FAILURE", argc);
        printf("Usage: open-with-append-seek <filename> <position to try seeking> <string to write>\n");
        cakelog_stop();
        exit(EXIT_FAILURE);
    }

    int seek_pos = atoi(argv[2]);
    cakelog("Set seek position to [%d]", seek_pos);

    int fd = open(argv[1], O_APPEND | O_WRONLY);

    if ( fd == -1 ) {
        cakelog("Unable to open file, errno is [%d]", errno);
        perror("open()");
        cakelog_stop();
        exit(EXIT_FAILURE);
    }
    else {
        cakelog("Succesfully opened file with file descriptor [%d]", fd);
    }

    cakelog("Attempting to seem to position [%d]", seek_pos);

    int new_pos = lseek(fd, seek_pos, SEEK_SET);
    if (new_pos == -1) {
        cakelog("Seek failed, errno is [%d]", errno);
        perror("lseek()");
    }
    else {  
        cakelog("New seek position is now: [%d], ready to write.", new_pos);
    }

    ssize_t bytes_written = write(fd, argv[3], strlen(argv[3]));

    if (bytes_written == -1) {
        cakelog("Error writing bytes, errno is [%d]", errno);
        perror("write()");
    }
    else {
        cakelog("[%ld] bytes written succesfully", bytes_written);
    }

    close(fd);

    if (cakelog_stop() != 0) {
        printf("There was a problem stopping cakelog, might want to look into it.\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
