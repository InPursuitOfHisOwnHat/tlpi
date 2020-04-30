#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "../../cakelog/cakelog.h"

#define BUF_SIZE 1024

int main(int argc, char** argv) {

    if (initialise_cakelog("fh_copy") != 0) {
        printf("main() : error initialising logging. Fast fail.\n");
        exit(EXIT_FAILURE);
    }

    if (argc < 3) {
        cakelog("Incorrect number of parameters passed, terminating with EXIT_FAILURE");
        printf("Usage: fh_copy <source filename> <destination filename>\n");
        exit(EXIT_FAILURE);
    }

    char *source_file = argv[1];
    char *dest_file = argv[2];

    cakelog("Source file is [%s], destination file is [%s]", source_file, dest_file);

    int sourcefd = open(source_file, O_RDONLY);

    if (sourcefd == -1) {
        cakelog("Couldn't open the file [%s], terminating", source_file);
        perror("Open source file");
        stop_cakelog();
        exit(EXIT_FAILURE);
    }

    cakelog("Succesfully opened file [%s] with file descriptor [%d]", source_file, sourcefd);

    int destfd = open(dest_file, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (destfd == -1) {
        cakelog("Couldn't open the file [%s] for writing, terminating", dest_file);
        perror("Open destination file");
        stop_cakelog();
        exit(EXIT_FAILURE);
    }

    char read_buf[BUF_SIZE];
    ssize_t bytes_read;
    ssize_t bytes_written;

    while((bytes_read = read(sourcefd, &read_buf, BUF_SIZE)) > 0) {
        cakelog("Read [%ld] bytes from sourcefile", bytes_read);
        bytes_written = write(destfd, &read_buf, bytes_read);
        cakelog("Written [%ld] bytes to destination file", bytes_written);
    }

    close(sourcefd);
    close(destfd);

    if (bytes_read < 0) {
        cakelog("There was an error at some point while copy the files.");
        perror("file copy");
        stop_cakelog();
        exit(EXIT_FAILURE);
    }

    printf("Copied [%s] to [%s]\n", source_file, dest_file);
    cakelog("Finished copying [%s] to [%s]", source_file, dest_file);

    stop_cakelog();

}
