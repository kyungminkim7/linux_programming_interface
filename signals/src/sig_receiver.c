#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static long sigCount[NSIG];
static volatile sig_atomic_t gotSigInt = 0;
static void sigHandler(int sig);

int main(int argc, char *argv[]) {
    printf("%s: PID is %ld\n", argv[0], (long) getpid());

    return EXIT_SUCCESS;
}

void sigHandler(int sig) {
    switch (sig) {
    case SIGINT: gotSigInt = 1; break;
    default: ++sigCount[sig]; break;
    }
}
