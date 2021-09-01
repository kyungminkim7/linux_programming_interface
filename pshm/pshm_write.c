#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <shm_name> <msg>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *shmName = argv[1];
    int shmFd = shm_open(shmName, O_RDWR, 0);
    if (shmFd == -1) {
        fprintf(stderr, "Failed to shm_open() %s - %s", shmName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    const char *msg = argv[2];
    size_t msgLength = strlen(msg);
    if (ftruncate(shmFd, msgLength) == -1) {
        fprintf(stderr, "Failed to ftruncate() %s to %ld bytes - %s", shmName, (long) msgLength, strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("Resized %s to %ld bytes\n", shmName, msgLength);

    char *addr = mmap(NULL, msgLength, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if (addr == MAP_FAILED) {
        fprintf(stderr, "Failed to mmap() %s - %s\n", shmName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (close(shmFd) == -1) {
        fprintf(stderr, "Failed to close() %s - %s", shmName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Copying %ld bytes\n", (long) msgLength);
    memcpy(addr, msg, msgLength);

    return EXIT_SUCCESS;
}
