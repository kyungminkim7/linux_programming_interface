#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char **environ;

int main(int argc, char *argv[]) {
    if (clearenv() != 0) {
        fprintf(stderr, "Failed to clear env - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; ++i) {
        if (putenv(argv[i]) != 0) {
            fprintf(stderr, "Failed to put env %s - %s\n", argv[i], strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if (setenv("GREET", "Hello World", 0) == -1) {
        fprintf(stderr, "Failed to put env %s - %s\n", "GREET", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (unsetenv("BYE") == -1) {
        fprintf(stderr, "Failed to unset env %s - %s\n", "BYE", strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (char **p = environ; *p != NULL; ++p) {
        puts(*p);
    }

    return 0;
}
