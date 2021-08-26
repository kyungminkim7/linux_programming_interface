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
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *pathname = argv[1];
    int fd = open(pathname, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open() %s - %s\n", pathname, strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct stat statbuf;
    if (fstat(fd, &statbuf) == -1) {
        fprintf(stderr, "Failed to fstat() %s - %s\n", pathname, strerror(errno));
        exit(EXIT_FAILURE);
    }

    char *addr = mmap(NULL, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        fprintf(stderr, "Failed to mmap() %s - %s\n", pathname, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1) {
        fprintf(stderr, "Failed to close() %s - %s\n", pathname, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (write(STDOUT_FILENO, addr, statbuf.st_size) != statbuf.st_size) {
        fprintf(stderr, "Failed to write() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (munmap(addr, statbuf.st_size) == -1) {
        fprintf(stderr, "Failed to munmap() %s - %s\n", pathname, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
