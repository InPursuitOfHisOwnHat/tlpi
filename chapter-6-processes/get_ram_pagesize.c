#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {

    long page_size = sysconf(_SC_PAGE_SIZE);
    printf("RAM page size on this system is: %ld bytes\n", page_size);
    exit(EXIT_SUCCESS);
}