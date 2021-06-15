#include <lpi/SignalFunctions.h>

#include <string.h>

void printSigSet(FILE *file, const char *prefix, const sigset_t *sigSet) {
    int count = 0;
    for (int sig = 1; sig < NSIG; ++sig) {
        switch (sigismember(sigSet, sig)) {
        case 1:
            fprintf(file, "%s%d (%s)\n", prefix, sig, strsignal(sig));
            ++count;
            break;
        default:
            break;
        }
    }

    if (count == 0) {
        fprintf(file, "%s<empty signal set>\n", prefix);
    }
}

int printSigMask(FILE *file, const char *msg) {
    if (msg != NULL) {
        fprintf(file, "%s", msg);
    }

    sigset_t sigMask;
    if (sigprocmask(SIG_BLOCK, NULL, &sigMask) == -1) {
        return -1;
    }

    printSigSet(file, "\t\t", &sigMask);
    return 0;
}

int printPendingSigs(FILE *file, const char *msg) {
    if (msg != NULL) {
        fprintf(file, "%s", msg);
    }

    sigset_t pendingSigs;
    if (sigpending(&pendingSigs) == -1) {
        return -1;
    }

    printSigSet(file, "\t\t", &pendingSigs);
    return 0;
}
