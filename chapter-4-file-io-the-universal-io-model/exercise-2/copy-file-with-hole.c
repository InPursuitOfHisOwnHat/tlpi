#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "cakelog/cakelog.h"

#define BUF_SIZE 1024

// cp seems to detect 'sparse' files, anyway, and I think the read/write system calls do, too.

// ‘--sparse=WHEN’
//      A “sparse file” contains “holes”—a sequence of zero bytes that does
//      not occupy any physical disk blocks; the ‘read’ system call reads
//      these as zeros.  This can both save considerable disk space and
//      increase speed, since many binary files contain lots of consecutive
//      zero bytes.  By default, ‘cp’ detects holes in input source files
//      via a crude heuristic and makes the corresponding output file
//      sparse as well.  Only regular files may be sparse.

int main(int argc, char** argv) {

    if (cakelog_initialise("fh_copy") != 0) {
        printf("main() : error initialising logging. Fast fail.\n");
        exit(EXIT_FAILURE);
    }

    if (argc < 3) {
        cakelog("Incorrect number of parameters passed, terminating with EXIT_FAILURE");
        printf("Usage: copy-file-with-hole <source filename> <destination filename>\n");
        exit(EXIT_FAILURE);
    }

    char *source_file = argv[1];
    char *dest_file = argv[2];

    cakelog("Source file is [%s], destination file is [%s]", source_file, dest_file);

    int sourcefd = open(source_file, O_RDONLY);

    if (sourcefd == -1) {
        cakelog("Couldn't open the file [%s], terminating", source_file);
        perror("Open source file");
        cakelog_stop();
        exit(EXIT_FAILURE);
    }

    cakelog("Succesfully opened file [%s] with file descriptor [%d]", source_file, sourcefd);

    int destfd = open(dest_file, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (destfd == -1) {
        cakelog("Couldn't open the file [%s] for writing, terminating", dest_file);
        perror("Open destination file");
        cakelog_stop();
        exit(EXIT_FAILURE);
    }

    char read_buf[BUF_SIZE];
    ssize_t bytes_read;
    ssize_t bytes_written;


    // Keep count so we can write useful results to our log at the end
    int total_null_bytes_read = 0;
    int curr_null_bytes_read = 0;
    int total_bytes_written = 0;

    while((bytes_read = read(sourcefd, &read_buf, BUF_SIZE)) > 0) {

        cakelog("Read [%ld] bytes from sourcefile", bytes_read);

        for (int i=0; i<bytes_read; i++) {
            if (read_buf[i] == '\0') {
                cakelog("Found Null at pos [%d]", i);
                curr_null_bytes_read++;
                total_null_bytes_read++;
            }
            else {
                cakelog("Seeking to [%d] bytes from end of file to write", curr_null_bytes_read);
                off_t seek_offset = lseek(destfd, curr_null_bytes_read, SEEK_END);
                bytes_written = write(destfd, &read_buf[i], 1);
                total_bytes_written += bytes_written;
                curr_null_bytes_read = 0;
            }
        }
    }

    cakelog("Found [%d] null bytes on read", total_null_bytes_read);
    cakelog("Written total of [%ld] bytes of actual data to destination file", total_bytes_written);
    
    close(sourcefd);
    close(destfd);

    if (bytes_read < 0) {
        cakelog("There was an error at some point while copying the files.");
        perror("file copy");
        cakelog_stop();
        exit(EXIT_FAILURE);
    }

    printf("Copied [%s] to [%s]\n", source_file, dest_file);
    cakelog("Finished copying [%s] to [%s]", source_file, dest_file);

    cakelog_stop();

}
