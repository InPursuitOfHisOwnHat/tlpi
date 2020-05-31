/*  Modify the listing in 7-1 to print out the current value of the program
    break after each malloc. Run the program specifying a small allocation
    block size.

    I'm not doing this using the listing - just going to write a simpler program
    that mallocs(). I think a lot of the code in this book is overengineered and
    obscures some of the points being made.
    
    This answer is just one big, fat memory leak.

    Run strace and you can see when it calls brk() to extend the breakpoint.

    For the example below, brk is called after block 907 is alllocated to 
    extend from 0x1e4d000 to 0x1e6e000

        = 0x1e6e000 - 0x1e4d000
        = 0x21000
        = 135168 bytes
        = 132K

write(1, "Allocated block 906, sbrk is now"..., 43Allocated block 906, sbrk is now 0x1e4d000) = 43
write(1, "Allocated block 907, sbrk is now"..., 43Allocated block 907, sbrk is now 0x1e4d000) = 43
brk(0x1e6e000)                          = 0x1e6e000
write(1, "Allocated block 908, sbrk is now"..., 43Allocated block 908, sbrk is now 0x1e6e000) = 43

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char** argv) {

    if (argc < 3) {
        printf("Usage: mem_alloc <block size (bytes)> <number of times>\n");
        exit(EXIT_FAILURE);
    }

    printf("Starting run, allocating block size of %s bytes %s times and sbrk() is %p\n",
                                                argv[1], argv[2], sbrk(0));

    char* block;
    int num_of_times = atoi(argv[2]);
    int block_size = atoi(argv[1]);
    for( int i=0; i<num_of_times; i++) {
        block = malloc(block_size);
        printf("Allocated block %d, sbrk is now %p\n", i, sbrk(0));
    }

    printf("Going to pause here so you can check memory usage.");
    getchar();

}


