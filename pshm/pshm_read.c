#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <shm_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *shmName = argv[1];
    int shmFd = shm_open(shmName, O_RDONLY, 0);
    if (shmFd == -1) {
        fprintf(stderr, "Failed to shm_open() %s - %s\n", shmName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct stat statbuf;
    if (fstat(shmFd, &statbuf) == -1) {
        fprintf(stderr, "Failed to fstat() %s - %s\n", shmName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    char *addr = mmap(NULL, statbuf.st_size, PROT_READ, MAP_SHARED, shmFd, 0);
    if (addr == MAP_FAILED) {
        fprintf(stderr, "Failed to mmap() %s - %s\n", shmName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (close(shmFd) == -1) {
        fprintf(stderr, "Failed to close() %s - %s\n", shmName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (write(STDOUT_FILENO, addr, statbuf.st_size) != statbuf.st_size) {
        fprintf(stderr, "Failed to write() %ld bytes - %s\n", (long) statbuf.st_size, strerror(errno));
        exit(EXIT_FAILURE);
    }
    write(STDOUT_FILENO, "\n", 1);

    return EXIT_SUCCESS;
}
