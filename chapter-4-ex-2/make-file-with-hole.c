#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include "cakelog/cakelog.h"

int main(int argc, char** argv) {

    if (cakelog_initialise("filehole") != 0) {
        printf("main() : error initialising logging. Fast fail.\n");
        exit(EXIT_FAILURE);
    }

    if ( argc < 2) {
        cakelog("Incorrect number of parameters passed ([%d]), terminating with EXIT_FAILURE", argc);
        printf("Usage: make-file-with-hole <filename> <size of hole in bytes>\n");
        cakelog_stop();
        exit(EXIT_FAILURE);
    }

    off_t bytes_to_seek = (off_t)atol(argv[2]);

    cakelog("Opening file [%s] for writing", argv[1]);

    int fd = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if ( fd == -1 ) {
        perror("Create file");
        cakelog_stop();
        exit(EXIT_FAILURE);
    }

    errno = 0;
    cakelog("Writing FHS into header");
    ssize_t bytes_written = write(fd, "FHS", 3);
    if (bytes_written == -1){
        perror("Writing initial bytes to file");
        cakelog_stop();
        exit(EXIT_FAILURE);
    }

    cakelog("Written [%ld] bytes to file", bytes_written);
    cakelog("Seeking [%ld] bytes ahead from end of file", bytes_to_seek);

    off_t seek_offset = lseek(fd, bytes_to_seek, SEEK_END);

    cakelog("Seeked [%ld] bytes after the end of the file, now at position [%ld]", bytes_to_seek, seek_offset);
    cakelog("Writing SHF bytes to end of file");

    ssize_t final_bytes_written = write(fd, "SHF", 3);
    if (final_bytes_written == -1){
        perror("Writing final bytes to file");
        cakelog_stop();
        exit(EXIT_FAILURE);
    }
    cakelog("Written final [%ld] bytes to file", final_bytes_written);
    cakelog("Done");

    cakelog_stop();
    close(fd);
    exit(EXIT_SUCCESS);
}
