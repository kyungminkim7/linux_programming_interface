#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void sigHandler(int sig) {
    static int count = 0;

    switch(sig) {
    case SIGINT:
        printf("Caught SIGINT - (%d)\n", ++count);
        break;
    case SIGQUIT:
        printf("Caught SIGQUIT - that's all folks!\n");
        exit(EXIT_SUCCESS);
    default:
        fprintf(stderr, "Caught unhandled signal %d\n", sig);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (signal(SIGINT, sigHandler) == SIG_ERR) {
        fprintf(stderr, "Failed to set signal handler for SIGINT - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (signal(SIGQUIT, sigHandler) == SIG_ERR) {
        fprintf(stderr, "Failed to set signal handler for SIGQUIT - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    for(;;) pause();

    return EXIT_SUCCESS;
}
