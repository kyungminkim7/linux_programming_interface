#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <lpi/SignalFunctions.h>

static volatile sig_atomic_t canJump = 0;

#ifdef USE_SIGSETJUMP
static sigjmp_buf env;
#else
static jmp_buf env;
#endif

static void handler(int sig);

int main(int argc, char *argv[]) {
    // Set handler
    struct sigaction action;
    action.sa_handler = handler;
    if (sigemptyset(&action.sa_mask) == -1) {
        fprintf(stderr, "Failed to set mask to sigemptyset() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    action.sa_flags = 0;

    if (sigaction(SIGINT, &action, NULL) == -1) {
        fprintf(stderr, "Failed to set sigaction() for SIGINT - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

#ifdef USE_SIGSETJUMP
    printf("Calling sigsetjmp()\n");
    if (sigsetjmp(env, 1) == 0) {
#else
    printf("Calling setjmp()\n");
    if (setjmp(env) == 0) {
#endif
        canJump = 1;
    } else {
        printSigMask(stdout, "After jump from handler, signal mask is:\n");
    }

    for (;;) {
        pause();
    }

    return EXIT_FAILURE;
}

void handler(int sig) {
    printf("Received signal %d (%s), signal mask is:\n", sig, strsignal(sig));
    printSigMask(stdout, NULL);

    if (!canJump) {
        printf("'env' buffer not yet set, doing a simple return\n");
        return;
    }

#ifdef USE_SIGSETJMP
    siglongjmp(env, 1);
#else
    longjmp(env, 1);
#endif
}
