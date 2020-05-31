
/*  Compile the program in listing 6-1 (mem_segments.c) and list its size using
    ls -l. Although the program contains an array (mbuf) that is around 10MB in
    sixe, the executable file is much smaller than this. Why is this?

    A: The buffer is for ephemeral memory, not storage space on disk.
    There's nothing to store until the program starts.
    
    This information is kept in the uninitialized data segmentand is
    initialized only when the program starts up. */