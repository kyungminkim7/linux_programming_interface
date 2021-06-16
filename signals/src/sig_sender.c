#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static long getLong(const char *str);

int main(int argc, char *argv[]) {
    const char *program = argv[0];
    if (argc < 4 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <pid> <num sigs> <sig1> [<sig2>]\n", program);
        exit(EXIT_FAILURE);
    }
    
    pid_t pid = getLong(argv[1]);
    long numSigs = getLong(argv[2]);
    int sig = getLong(argv[3]);

    printf("%s: sending signal %d to process %ld %ld times\n",
        program, sig, (long) pid, numSigs);
    for (long i = 0; i < numSigs; ++i) {
        if (kill(pid, sig) == -1) {
            fprintf(stderr, "Failed to send signal %d to process %ld - %s\n", 
                sig, (long) pid, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if (argc > 4) {
        sig = getLong(argv[4]);
        if (kill(pid, sig) == -1) {
            fprintf(stderr, "Failed to send signal %d to process %ld - %s\n", 
                sig, (long) pid, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    printf("%s: exiting\n", program);
    return EXIT_SUCCESS;
}

long getLong(const char *str) {
    char *endptr;
    errno = 0;
    long result = strtol(str, &endptr, 0);
    if (errno != 0 || endptr == str) {
        fprintf(stderr, "Failed to convert strtol() %s\n", str);
        exit(EXIT_FAILURE);
    }
    return result;
}
