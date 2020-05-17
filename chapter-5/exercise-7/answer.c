// Implement readv() and writev() using read(), write() and suitable functions from the malloc package
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "cakelog.h"

#define STRING_LEN 255

int my_writev(int fd, const struct iovec* iov, int iovcnt) {

    // size up buffer
    cakelog("my_writev(): building buffer, iovcnt = %d", iovcnt);
    ssize_t size_of_buffer = 0;
    for (int i=0; i<iovcnt; i++) {
        size_of_buffer += iov[i].iov_len;
    }
    cakelog("my_writev(): buffer will be %d bytes long", size_of_buffer);

    // allocate buffer
    void* buff = calloc(1,size_of_buffer);
    if ( buff == NULL) {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    cakelog("my_writev(): allocated buffer of %d bytes", size_of_buffer);

    int buff_pointer = 0;
    for (int i=0; i<iovcnt; i++) {
        cakelog("my_writev(): copying iov[%d].iov_base ([%s]), size %d into buffer using memcpy", i, iov[i].iov_base,iov[i].iov_len);
        memcpy(&buff[buff_pointer], iov[i].iov_base, iov[i].iov_len);
        buff_pointer += iov[i].iov_len;
    }

    cakelog("my_writev(): written buffer");

    int bytes_written = write(fd, buff, size_of_buffer);
    if (bytes_written == -1) {
        perror("write()");
        exit(EXIT_FAILURE);
    }
    cakelog("my_writev(): written %d bytes to out file.", bytes_written);

    free(buff);
    return 0;
}

int my_readv(int fd, struct iovec* iov, int iovcnt) {

    cakelog("my_readv(): building buffer");
    ssize_t size_of_buffer = 0;
    for (int i=0; i<iovcnt; i++) {
        size_of_buffer += iov[i].iov_len;
    }
    cakelog("my_readv(): buffer size will be %d bytes long", size_of_buffer);

    // allocate buffer
    void* buff = malloc(size_of_buffer);
    cakelog("my_readv(): allocated buffer of %d bytes", size_of_buffer);

    int bytes_read = read(fd, buff, size_of_buffer);
    if (bytes_read == -1) {
        perror("read()");
        exit(EXIT_FAILURE);
    }
    cakelog("my_readv(): read %d bytes into buffer", bytes_read);
    
    int buff_pointer = 0;
    for (int i=0; i<iovcnt; i++) {
        cakelog("my_readv(): copying iov[%d].iov_base, size %d into buffer using memcpy", i, iov[i].iov_len);
        memcpy(iov[i].iov_base, &buff[buff_pointer], iov[i].iov_len);
        buff_pointer += iov[i].iov_len;
        cakelog("my_readv(): buff_pointer is now [%d]", buff_pointer);
    }

    return 0;
}

int main(int argc, char** argv) {

    cakelog_initialise("my-writev", false);

    // Write some stuff to a file using scatter
    char first_word[STRING_LEN] = "Hello";
    int second_word = 100;
    char third_word[STRING_LEN] = "You!";

    struct iovec iov_w[3];

    iov_w[0].iov_base = first_word;
    iov_w[0].iov_len = STRING_LEN;

    iov_w[1].iov_base = &second_word;
    iov_w[1].iov_len = sizeof(second_word);

    iov_w[2].iov_base = third_word;
    iov_w[2].iov_len = STRING_LEN;

    int fd_w = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd_w == -1) {
        perror("open()");
        exit(EXIT_FAILURE);
    }

    // ************************
    my_writev(fd_w, iov_w, 3);
    // ************************

    close(fd_w);

    // Now read it all back in
    char str_1[STRING_LEN];
    int n;
    char str_2[STRING_LEN];

    struct iovec iov_r[3];
    iov_r[0].iov_base = &str_1;
    iov_r[0].iov_len = STRING_LEN;

    iov_r[1].iov_base = &n;
    iov_r[1].iov_len = sizeof(n);

    iov_r[2].iov_base = &str_2;
    iov_r[2].iov_len = STRING_LEN;
 
    int fd_r = open(argv[1], O_RDONLY);
    if (fd_r == -1) {
        perror("open()");
        exit(EXIT_FAILURE);
    }

    // ************************
    my_readv(fd_r, iov_r, 3);
    // ************************

    printf("Here's what we wrote to the file:\n");
    printf("->%s\n", (char*)iov_r[0].iov_base);
    printf("->%d\n", *(int*)iov_r[1].iov_base);
    printf("->%s\n", (char*)iov_r[2].iov_base);

    close(fd_r);
    cakelog_stop();
}