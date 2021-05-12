#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void printFpathconf(const char *msg, int fd, int name);

int main(int argc, char *argv[]) {
    printFpathconf("_PC_NAME_MAX", STDIN_FILENO, _PC_NAME_MAX);
    printFpathconf("_PC_PATH_MAX", STDIN_FILENO, _PC_PATH_MAX);
    printFpathconf("_PC_PIPE_BUF", STDIN_FILENO, _PC_PIPE_BUF);

    return 0;
}

void printFpathconf(const char *msg, int fd, int name) {
    errno = 0;
    long limit = fpathconf(fd, name);
    if (limit == -1) {
        if (errno == 0) {
            fprintf(stderr, "%s: indeterminate\n", msg);
        } else {
            fprintf(stderr, "Failed to get limit for %s - %s\n", msg, strerror(errno));
            exit(EXIT_FAILURE);
        }
    } else {
        printf("%s: %ld\n", msg, limit);
    }
}
