#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define PIPE_READ_FD 0
#define PIPE_WRITE_FD 1

static unsigned int getUint(const char *str);

int main(int argc, char *argv[]) {
    if (argc <= 1 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <sec>...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    setbuf(stdout, NULL);
    printf("Parent started\n");

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        fprintf(stderr, "Failed to create pipe() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; ++i) {
        switch(fork()) {
        case -1:
            fprintf(stderr, "Failed to fork() child process - %s\n", strerror(errno));
            exit(EXIT_FAILURE);

        case 0:
            if (close(pipefd[PIPE_READ_FD]) == -1) {
                fprintf(stderr, "Child failed to close pipe read fd - %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            
            sleep(getUint(argv[i]));

            printf("Child %d (PID=%ld) closing pipe\n", i, (long) getpid());
            if (close(pipefd[PIPE_WRITE_FD]) == -1) {
                fprintf(stderr, "Child failed to close pipe write fd - %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

            _exit(EXIT_SUCCESS);

        default:
            break;
        }
    }

    if (close(pipefd[PIPE_WRITE_FD]) == -1) {
        fprintf(stderr, "Parent failed to close pipe write fd - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char buffer;
    if (read(pipefd[PIPE_READ_FD], &buffer, 1) != 0) {
        fprintf(stderr, "Parent didn't get EOF - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (close(pipefd[PIPE_READ_FD]) == -1) {
        fprintf(stderr, "Parent failed to close pipe read fd - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Parent ready to go\n");

    for (int i = 1; i < argc; ++i) {
        if (wait(NULL) == -1) {
            fprintf(stderr, "Failed to wait() on child - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    return EXIT_SUCCESS;
}

unsigned int getUint(const char *str) {
    char *endptr;
    errno = 0;
    unsigned int result = strtoul(str, &endptr, 0);
    if (errno != 0 && *endptr != '\0') {
        fprintf(stderr, "Failed to convert %s to unsigned int - %s\n", str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}
