#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static volatile unsigned long handlerSleepTime = 0;
static volatile unsigned long numSigsCaught = 0;
static volatile sig_atomic_t isDone = 0;

static unsigned long getUnsignedLong(const char *str);
static void sigHandler(int signal, siginfo_t *sigInfo, void *context);

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s [<block time> [<handler sleep time>]]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("%s: PID is %ld\n", argv[0], (long) getpid());
    handlerSleepTime = (argc > 2) ? getUnsignedLong(argv[2]) : 1;

    struct sigaction action;
    if(sigemptyset(&action.sa_mask) == -1) {
        fprintf(stderr, "Failed to set empty signal handler mask - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    action.sa_sigaction = sigHandler;
    action.sa_flags = SA_SIGINFO;

    for (int i = 1; i < NSIG; ++i) {
        if (i != SIGTSTP && i != SIGQUIT) {
            sigaction(i, &action, NULL);
        }
    }

    if (argc > 1) {
        sigset_t prevMask, mask;
        if (sigfillset(&mask) == -1) {
            fprintf(stderr, "Failed to sigfillset() process mask - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        const int allowedSignals[] = {SIGINT, SIGQUIT};
        for (int i = 0; i < sizeof(allowedSignals) / sizeof(int); ++i) {
            if (sigdelset(&mask, allowedSignals[i]) == -1) {
                fprintf(stderr, "Failed to sigdelset() %s from process mask - %s", 
                    strsignal(allowedSignals[i]), strerror(errno));
                exit(EXIT_FAILURE);
            }
        }

        if (sigprocmask(SIG_SETMASK, &mask, &prevMask) == -1) {
            fprintf(stderr, "Failed to set sigprocmask() - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        const unsigned long blockTime = getUnsignedLong(argv[1]);
        printf("%s: signals blocked - sleeping %lu seconds\n", argv[0], blockTime);
        sleep(blockTime);
        printf("%s: sleep complete\n", argv[0]);

        if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1) {
            fprintf(stderr, "Failed to restore sigprocmask() - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    while (!isDone) {
        pause();
    }

    printf("Caught %lu signals\n", numSigsCaught);
    return EXIT_SUCCESS;
}

unsigned long getUnsignedLong(const char *str) {
    char *endptr;
    errno = 0;
    unsigned long result = strtoul(str, &endptr, 0);
    if (errno != 0 || *endptr != '\0') {
        fprintf(stderr, "Failed convert %s to unsigned long - %s\n", str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}

void sigHandler(int signal, siginfo_t *sigInfo, void *context) {
    if (signal == SIGINT || signal == SIGTERM) {
        isDone = 1;
        return;
    }

    printf("Caught signal %d\n", signal);
    printf("\tsi_signo=%d, si_code=%d (%s), si_value=%d\n",
        sigInfo->si_signo,
        sigInfo->si_code,
        (sigInfo->si_code == SI_USER) ? "SI_USER" :
        (sigInfo->si_code == SI_QUEUE) ? "SI_QUEUE" : "other",
        sigInfo->si_value.sival_int);
    printf("\tsi_pid=%ld, si_uid=%ld\n\n", (long) sigInfo->si_pid, (long) sigInfo->si_uid);
    sleep(handlerSleepTime);

    ++numSigsCaught;
}
