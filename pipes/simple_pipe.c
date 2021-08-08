#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 10

int main(int argc, char *argv[]) {
    if (argc != 2 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <str>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fd[2];
    if (pipe(fd) == -1) {
        fprintf(stderr, "Failed to create pipe() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    ssize_t numRead;

    switch(fork()) {
    case -1:
        fprintf(stderr, "Failed to fork() child - %s\n", strerror(errno));
        exit(EXIT_FAILURE);

    case 0: // Child
        if (close(fd[1]) == -1) {
            fprintf(stderr, "CHILD: Failed to close() pipe's write fd - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        while ((numRead = read(fd[0], buffer, BUFFER_SIZE)) > 0) {
            if (write(STDOUT_FILENO, buffer, numRead) != numRead) {
                fprintf(stderr, "CHILD: Failed to write() to pipe - %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
        write(STDOUT_FILENO, "\n", 1);

        if (numRead == -1) {
            fprintf(stderr, "CHILD: Failed to read() from pipe - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (close(fd[0]) == -1) {
            fprintf(stderr, "CHILD: Failed to close() pipe's read fd - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        _exit(EXIT_SUCCESS);

    default: // Parent
        if (close(fd[0]) == -1) {
            fprintf(stderr, "PARENT: Failed to close() pipe's read fd - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (write(fd[1], argv[1], strlen(argv[1])) != strlen(argv[1])) {
            fprintf(stderr, "PARENT: Failed to write() to pipe - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (close(fd[1]) == -1) {
            fprintf(stderr, "PARENT: Failed to close() pipe's write fd - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (wait(NULL) == -1) {
            fprintf(stderr, "PARENT: Failed to wait() on child - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);
    }
}
