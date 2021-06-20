#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void handler(int sig);
static void overflowStack(int i);

int main(int argc, char *argv[]) {
    int j;
    printf("Top of std stack is near %10p\n", &j);

    // Set sigaltstack
    stack_t stack;
    if ((stack.ss_sp = malloc(SIGSTKSZ)) == NULL) {
        fprintf(stderr, "Failed to malloc() %ld bytes for sigaltstack - %s\n", 
            (long) SIGSTKSZ, strerror(errno));
        exit(EXIT_FAILURE);
    }
    stack.ss_flags = 0;
    stack.ss_size = SIGSTKSZ;

    if (sigaltstack(&stack, NULL) == -1) {
        fprintf(stderr, "Failed to set sigaltstack() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Alternate stack is at    %10p-%p\n",
        stack.ss_sp, sbrk(0) - 1);

    struct sigaction action;
    action.sa_handler = handler;
    if (sigemptyset(&action.sa_mask) == -1) {
        fprintf(stderr, "Failed to set sigaction mask to emptysigset() - %s\n", 
            strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGSEGV, &action, NULL) == -1) {
        fprintf(stderr, "Failed to set sigaction() for SIGSEGV - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    overflowStack(1);

    return EXIT_SUCCESS;
}

void handler(int sig) {
    int j;
    printf("Caught signal %d (%s)\n", sig, strsignal(sig));
    printf("Top of handler stack is near %10p\n", &j);
    fflush(NULL);
    _exit(EXIT_FAILURE);
}

void overflowStack(int i) {
    char tmp[100000];
    printf("Call %4d - top of stack near %10p\n", i, &tmp[0]);
    overflowStack(i + 1);
}
