#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define TIME_STR_BUFFER_SIZE 500

static unsigned int getUint(const char *str);
static int getCurrTimeStr(char *buffer, ssize_t bufferSize);

int main(int argc, char *argv[]) {
    if (argc < 2 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <sleep-time>...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    setbuf(stdout, NULL);
    for (int i = 1; i < argc; ++i) {
        switch (fork()) {
        case -1:
            fprintf(stderr, "Failed to fork() child process - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        case 0:
            // Child process
            {
                char timeStr[TIME_STR_BUFFER_SIZE];
                if (getCurrTimeStr(timeStr, TIME_STR_BUFFER_SIZE) == -1) {
                    fprintf(stderr, "Failed to get current time - %s", strerror(errno));
                    _exit(EXIT_FAILURE);
                }

                printf("[%s] child %d started with PID %ld, sleeping %s seconds\n",
                    timeStr, i, (long) getpid(), argv[i]);
            }
            sleep(getUint(argv[i]));
            _exit(EXIT_SUCCESS);
        default:
            // Parent process
            break;
        }
    }

    pid_t childPid;
    char timeStr[TIME_STR_BUFFER_SIZE];
    for (int i = 1; (childPid = wait(NULL)) != -1; ++i) {
        if (getCurrTimeStr(timeStr, TIME_STR_BUFFER_SIZE) == -1) {
            fprintf(stderr, "Failed to get current time - %s", strerror(errno));
            _exit(EXIT_FAILURE);
        }

        printf("[%s] wait() returned child PID %ld (numDead=%d)\n",
            timeStr, (long) childPid, i);
    }

    if (errno != ECHILD) {
        fprintf(stderr, "Failed to wait() for child process - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    


    return EXIT_SUCCESS;
}

unsigned int getUint(const char *str) {
    char *endptr;
    errno = 0;
    unsigned long result = strtoul(str, &endptr, 0);
    if (errno != 0 || *endptr != '\0') {
        fprintf(stderr, "Failed to convert %s strtoul() - %s\n", str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}

int getCurrTimeStr(char *buffer, ssize_t bufferSize) {
    time_t currTime = time(NULL);
    struct tm *currLocalTime = localtime(&currTime);
    if (currLocalTime == NULL) {
        return -1;
    }

    if (strftime(buffer, bufferSize, "%T", currLocalTime) == 0) {
        return -1;
    }

    return 0;
}
