#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void sigHandler(int sig) {
    printf("Ouch\n");
}

int main(int argc, char *argv[]) {
    if (signal(SIGINT, &sigHandler) == SIG_ERR) {
        fprintf(stderr, "Failed to set SIGINT handler - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (int i = 0; ; ++i) {
        printf("%d\n", i);
        sleep(3);
    }

    return EXIT_SUCCESS;
}
