#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <pathname> [<new-value>]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *pathname = argv[1];
    int fd = open(pathname, O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "Failed to open() %s - %s\n", pathname, strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct stat statbuf;
    if (fstat(fd, &statbuf) == -1) {
        fprintf(stderr, "Failed to fstat() %s - %s\n", pathname, strerror(errno));
        exit(EXIT_FAILURE);
    }

    char *addr = mmap(NULL, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        fprintf(stderr, "Failed to mmap() %s - %s\n", pathname, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1) {
        fprintf(stderr, "Failed to close() %s - %s\n", pathname, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Current string=%.*s\n", (int) statbuf.st_size, addr);

    if (argc > 2) {
        memset(addr, 0, statbuf.st_size);
        strncpy(addr, argv[2], statbuf.st_size - 1);
        if (msync(addr, statbuf.st_size, MS_SYNC) == -1) {
            fprintf(stderr, "Failed to msync() %s - %s\n", pathname, strerror(errno));
            exit(EXIT_FAILURE);
        }

        printf("Copied \"%s\" to shared memory\n", argv[2]);
    }

    if (munmap(addr, statbuf.st_size) == -1) {
        fprintf(stderr, "Failed to munmap() %s - %s\n", pathname, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    return EXIT_SUCCESS;
}
