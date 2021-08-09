#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define PIPE_READ_FD 0
#define PIPE_WRITE_FD 1

int main(int argc, char *argv[]) {
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        fprintf(stderr, "Failed to create pipe() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Create child to execute ls
    switch(fork()) {
        case -1:
            fprintf(stderr, "Failed to fork() child ls process - %s\n", strerror(errno));
            exit(EXIT_FAILURE);

        case 0:
            if (close(pipefd[PIPE_READ_FD]) == -1) {
                fprintf(stderr, "Child ls process failed to close() pipe read fd - %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

            if (pipefd[PIPE_WRITE_FD] != STDOUT_FILENO) {
                if (dup2(pipefd[PIPE_WRITE_FD], STDOUT_FILENO) == -1) {
                    fprintf(stderr, "Child ls process failed to dup2() pipe write fd to stdout - %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }

                if (close(pipefd[PIPE_WRITE_FD]) == -1) {
                    fprintf(stderr, "Child ls process failed to close() pipe write fd - %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }
            }

            if (execlp("ls", "ls", (char *) NULL) == -1) {
                fprintf(stderr, "Failed to execlp() ls - %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

        default: 
            break;
    }

    // Create child to exec wc
    switch(fork()) {
        case -1:
            fprintf(stderr, "Failed to fork() child wc process - %s\n", strerror(errno));
            exit(EXIT_FAILURE);

        case 0:
            if (close(pipefd[PIPE_WRITE_FD]) == -1) {
                fprintf(stderr, "Child wc process failed to close() pipe write fd - %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

            if (pipefd[PIPE_READ_FD] != STDIN_FILENO) {
                if (dup2(pipefd[PIPE_READ_FD], STDIN_FILENO) == -1) {
                    fprintf(stderr, "Child wc process failed to dup2() pipe read fd to stdin - %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }

                if (close(pipefd[PIPE_READ_FD]) == -1) {
                    fprintf(stderr, "Child wc process failed to close() pipe read fd - %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }
            }

            if (execlp("wc", "wc", "-l", (char *) NULL) == -1) {
                fprintf(stderr, "Failed to execlp() wc - %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

        default: 
            break;
    }

    if (close(pipefd[PIPE_READ_FD]) == -1) {
        fprintf(stderr, "Parent failed to close() pipe read fd - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (close(pipefd[PIPE_WRITE_FD]) == -1) {
        fprintf(stderr, "Parent failed to close() pipe write fd - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 2; ++i) {
        if (wait(NULL) == -1) {
            fprintf(stderr, "Parent failed to wait() on child - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}
