#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "cakelog.h"


int main(int argc, char** argv) {

    cakelog_initialise("answer", false);

    char* filename = argv[1];

    int fd1 = open(argv[1], O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    cakelog("Opened file %s with file descriptor %d", argv[1], fd1);

    char* test_bytes = "Hello, this is a test file!";
    int bytes_written = write(fd1, test_bytes, strlen(test_bytes));

    if (bytes_written == -1) {
        perror("write()");
        exit(EXIT_FAILURE);
    }

    cakelog("Written %d bytes to file", bytes_written);

    int fd2 = dup(fd1);
    cakelog("Duplicated file descriptor %d to %d", fd1, fd2);

    off_t fd1_offset = lseek(fd1, 0, SEEK_CUR);
    off_t fd2_offset = lseek(fd2, 0, SEEK_CUR);

    cakelog("FD %d is currently at offset %ld, FD %d is currently at offset %ld", fd1, fd1_offset, fd2, fd2_offset);
    cakelog("Going to seek to offset 10 on FD %d only", fd1);

    fd1_offset = lseek(fd1, 10, SEEK_SET);

    cakelog("FD %d has now been seeked and is at %ld, now getting current offset from %d", fd1, fd1_offset, fd2);

    fd2_offset = lseek(fd2, 0, SEEK_CUR);

    cakelog("Final result is: FD %d is currently at offset %ld, FD %d is currently at offset %ld, so dup file descriptors definitley share an offset", fd1, fd1_offset, fd2, fd2_offset);

    cakelog("Getting file status flags for both descriptors");

    int fd1_flags = fcntl(fd1, F_GETFL);
    int fd2_flags = fcntl(fd2, F_GETFL);

    cakelog("(just printing as INT because no specific actions in exercise, just prove they're the same) FD %d flags: %d, FD %d flags: %d", fd1, fd1_flags, fd2, fd2_flags);

    close(fd1);
    close(fd2);
    cakelog_stop();

}