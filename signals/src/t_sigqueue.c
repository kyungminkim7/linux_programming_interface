#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static long getLong(const char *str);

int main(int argc, char *argv[]) {
    if (argc < 4 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <pid> <sig> <data> [<num sigs>]\n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("%s: PID is %ld, UID is %ld\n", argv[0], (long) getpid(), (long) getuid());

    pid_t pid = getLong(argv[1]);
    int sig = getLong(argv[2]);
    int data = getLong(argv[3]);
    int numSigs = (argc > 4) ? getLong(argv[4]) : 1;

    for (int i = 0; i < numSigs; ++i) {
        union sigval value;
        value.sival_int = data + i;

        if (sigqueue(pid, sig, value) == -1) {
            fprintf(stderr, "Failed to sigqueue() - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}

long getLong(const char *str) {
    char *endptr;
    errno = 0;
    long result = strtol(str, &endptr, 0);
    if (errno != 0 || *endptr != '\0') {
        fprintf(stderr, "Failed to convert %s to long - %s\n", str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}
