#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <lpi/SignalFunctions.h>

static long sigCount[NSIG];
static volatile sig_atomic_t gotSigInt = 0;
static void sigHandler(int sig);
static unsigned int getUnsignedInt(const char *str);

int main(int argc, char *argv[]) {
    printf("%s: PID is %ld\n", argv[0], (long) getpid());

    for (int i = 1; i < NSIG; ++i) {
        signal(i, sigHandler);
    }

    if (argc > 1) {
        unsigned int sec = getUnsignedInt(argv[1]);

        // Block all signals
        sigset_t sigSet;
        if (sigfillset(&sigSet) == -1) {
            fprintf(stderr, "Failed to sigfillset() - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (sigprocmask(SIG_BLOCK, &sigSet, NULL) == -1) {
            fprintf(stderr, "Failed to block signals - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        printf("%s: Sleeping for %u secs\n", argv[0], sec);
        sleep(sec);

        // Print pending signals
        if (sigpending(&sigSet) == -1) {
            fprintf(stderr, "Failed to get pending signals - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        
        printf("%s: pending signals are: \n", argv[0]);
        printSigSet(stdout, "\t\t", &sigSet);

        // Unblock signals
        if (sigemptyset(&sigSet) == -1) {
            fprintf(stderr, "Failed to sigemptyset() - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (sigprocmask(SIG_SETMASK, &sigSet, NULL) == -1) {
            fprintf(stderr, "Failed to set signal mask - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    while (!gotSigInt);

    for (int i = 1; i < NSIG; ++i) {
        if (sigCount[i] != 0) {
            printf("%s: signal %d caught %ld time%s\n", 
                argv[0], i, sigCount[i], 
                sigCount[i] == 1 ? "" : "s");
        }
    }

    return EXIT_SUCCESS;
}

void sigHandler(int sig) {
    switch (sig) {
    case SIGINT: gotSigInt = 1; break;
    default: ++sigCount[sig]; break;
    }
}

unsigned int getUnsignedInt(const char *str) {
    char *endptr;
    errno = 0;
    unsigned int result = strtoul(str, &endptr, 0);
    if (errno != 0 || *endptr != '\0') {
        fprintf(stderr, "Failed to convert %s to unsigned long - %s\n", str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}
