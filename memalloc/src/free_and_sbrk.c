#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_ALLOCS 1000000

static unsigned long getUnsignedLong(const char *str);

int main(int argc, char *argv[]) {
    if (argc < 3 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <num-allocs> <block-size> [<step> [<min> [<max>]]]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    // Parse cmd line options
    unsigned long numAllocs = getUnsignedLong(argv[1]);
    if (numAllocs > MAX_ALLOCS) {
        fprintf(stderr, "ERROR: Num allocs > MAX_ALLOCS (%d)\n", MAX_ALLOCS);
        exit(EXIT_FAILURE);
    }
    unsigned long blockSize = getUnsignedLong(argv[2]);

    unsigned long freeStep = (argc > 3 ? getUnsignedLong(argv[3]) : 1);
    unsigned long freeMin = (argc > 4 ? getUnsignedLong(argv[4]) : 1);
    unsigned long freeMax = (argc > 5 ? getUnsignedLong(argv[5]) : numAllocs);
    if (freeMax > numAllocs) {
        fprintf(stderr, "ERROR: freeMax (%lu) > numAllocs (%lu)\n", freeMax, numAllocs);
        exit(EXIT_FAILURE);
    }

    // Allocate memory
    printf("Initial program break: %p\n\n", sbrk(0));

    printf("Allocating %lu * %lu bytes\n", numAllocs, blockSize);
    char *memBlocks[MAX_ALLOCS];
    for (unsigned long i = 0ul; i < numAllocs; ++i) {
        if ((memBlocks[i] = malloc(blockSize)) == NULL) {
            fprintf(stderr, "Failed to allocate %lu bytes - %s\n", blockSize, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    printf("Program break is now: %p\n\n", (char *) sbrk(0));

    // Free memory
    printf("Freeing blocks from %lu to %lu in steps of %lu\n", freeMin, freeMax, freeStep);
    for (unsigned long i = freeMin - 1; i < freeMax; i+=freeStep) {
        free(memBlocks[i]);
    }
    printf("Program break is now: %p\n\n", (char *) sbrk(0));

    return 0;
}

unsigned long getUnsignedLong(const char *str) {
    char *endptr = NULL;
    errno = 0;
    long n = strtoul(str, &endptr, 0);
    if (errno != 0 || endptr == str) {
        fprintf(stderr, "Failed to convert %s to unsigned long\n", str);
        exit(EXIT_FAILURE);
    }
    return n;
}
