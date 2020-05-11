#include "cakelog.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


int dup(int fd) {

    // We don't care about the new fd, just needs to be the first available one
    int new_fd = fcntl(fd, F_DUPFD, 0);
    cakelog("dup(): Got new FD of %d", new_fd);
    return new_fd;
}

int dup2(int old_fd, int new_fd) {

    cakelog("dup2(): Request to duplicate %d as %d", old_fd, new_fd);

    cakelog("dup2(): Checking to see if old FD is valid, first.");
    if (fcntl(old_fd, F_GETFL) == -1) {
        cakelog("dup2(): Old FD is not valid, returning -1");
        perror("dup2(): fcntl()");
        return -1;
    }

    cakelog("dup2(): Checking to see if new FD is already taken");
    if (fcntl(new_fd, F_GETFL) != -1) {
        cakelog("dup2(): New FD is taken, closing it.");
        close(new_fd);
    }

    cakelog("dup2(): Duplicating %d to %d", old_fd, new_fd);
    return fcntl(old_fd, F_DUPFD, new_fd);
}

int main() {

    cakelog_initialise("dup", false);

    // Just picked a random int
    int duplicate_fd = 1001;

    int stdout_fd = dup2(0, duplicate_fd);
    cakelog("main(): Got FD number duplicate %d by copying %d", stdout_fd, duplicate_fd);

    char* output = "Writing to duplicate file descriptor of STDOUT, so this should appear on STDOUT\n";
    cakelog("Now writing string to duplicate file descriptor");
    write(duplicate_fd, output, strlen(output));
    
    cakelog("Closing duplicate FD");
    close(stdout_fd);

    cakelog_stop();

}