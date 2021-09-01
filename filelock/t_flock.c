#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>

static void printUsage(const char *program);
static long getLong(const char *str);

int main(int argc, char *argv[]) {
    // Parse args
    if (argc < 3 || strcmp(argv[1], "-h") == 0) {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *pathname = argv[1];

    int op;
    switch (argv[2][0]) {
        case 's':
            op = LOCK_SH;
            break;

        case 'x':
            op = LOCK_EX;
            break;

        default:
            printUsage(argv[0]);
            exit(EXIT_FAILURE);
    }

    size_t opArgLength = strlen(argv[2]);
    if (opArgLength == 2) {
        if (argv[2][1] == 'n') {
            op |= LOCK_NB;
        } else {
            printUsage(argv[0]);
            exit(EXIT_FAILURE);
        }
    } else if (opArgLength > 2) {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    // Lock file
    int fd = open(pathname, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open() %s - %s\n", pathname, strerror(errno));
        exit(EXIT_FAILURE);
    }

    const char *lockName = (op & LOCK_SH) ? "LOCK_SH" : "LOCK_EX";
    printf("PID %ld: requesting %s at %s\n", (long) getpid(), lockName, pathname);

    if (flock(fd, op) == -1) {
        if (errno == EWOULDBLOCK) {
            fprintf(stderr, "PID %ld: already locked - bye!\n", (long) getpid());
        } else {
            fprintf(stderr, "Failed to flock() %s at %s - %s\n", lockName, pathname, strerror(errno));
        }
        exit(EXIT_FAILURE);
    }

    printf("PID %ld: granted %s at %s\n", (long) getpid(), lockName, pathname);
    sleep((argc > 3) ? getLong(argv[3]) : 10);

    printf("PID %ld: releasing %s at %s\n", (long) getpid(), lockName, pathname);
    if (flock(fd, LOCK_UN) == -1) {
        fprintf(stderr, "Failed to unlock flock() at %s - %s\n", pathname, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

void printUsage(const char *program) {
    fprintf(stderr, "Usage: %s <file> <lock> [<secs>]\n", program);
    fprintf(stderr, "    <lock> - 's' (shared) or 'x' (exclusive)\n");
    fprintf(stderr, "             optionally followed by 'n' (nonblocking)\n");
    fprintf(stderr, "    <secs> - seconds to hold the lock\n");
}

long getLong(const char *str) {
    char *endptr;
    errno = 0;
    long result = strtol(str, &endptr, 0);
    if (errno != 0 && *endptr != '\0') {
        fprintf(stderr, "Failed to convert strtol() %s - %s\n", str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}
