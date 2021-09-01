#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

int main(int argc, char *argv[]) {
    if (argc != 2 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <shm_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *shmName = argv[1];
    if (shm_unlink(shmName) == -1) {
        fprintf(stderr, "Failed to shm_unlink() %s", shmName);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
