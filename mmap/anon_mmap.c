#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char *addr = mmap(NULL, sizeof(int), 
        PROT_READ | PROT_WRITE, 
        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        fprintf(stderr, "Anonymous mmap() failed - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    *addr = 1;

    switch(fork()) {
    case -1:
        fprintf(stderr, "Failed to fork() child process - %s\n", strerror(errno));
        exit(EXIT_FAILURE);

    case 0:
        printf("Child started, value = %d\n", *addr);
        ++(*addr);
        if (munmap(addr, sizeof(int)) == -1) {
            fprintf(stderr, "Failed to munmap() - %s\n", strerror(errno));
            _exit(EXIT_FAILURE);
        }
        _exit(EXIT_SUCCESS);

    default: break;
    }

    if (wait(NULL) == -1) {
        fprintf(stderr, "Failed to wait() on child process - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("In parent, value = %d\n", *addr);

    if (munmap(addr, sizeof(int)) == -1) {
        fprintf(stderr, "Failed to munmap() - %s\n", strerror(errno));
        _exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
