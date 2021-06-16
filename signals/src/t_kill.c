#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int getInt(const char *str);

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <pid> <sig-num>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int pid = getInt(argv[1]);
    int sig = getInt(argv[2]);
    int result = kill(pid,sig); 
    if (sig == 0) {
        if (result == 0) {
            printf("Process exists and we can send it a signal\n");
        } else {
            switch (errno) {
            case EPERM:
                printf("Process %d exists, but we don't have permission to send signal %d\n", 
                    pid, sig);
                break;
            case ESRCH:
                printf("Process %d does not exist\n", pid);
                break;
            default:
                fprintf(stderr, "Failed to kill() %d with signal %d\n", pid, sig);
                exit(EXIT_FAILURE);
            }
        }
    } else if (result == -1) {
        fprintf(stderr, "Failed to kill() %d with signal %d\n", pid, sig);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

int getInt(const char *str) {
    char *endptr = NULL;
    errno = 0;
    long result = strtol(str, &endptr, 0);
    if (errno != 0 || endptr == str) {
        fprintf(stderr, "Failed to convert %s to long", str);
        exit(EXIT_FAILURE);
    }
    return result;
}
