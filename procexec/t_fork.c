#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static int iData = 111;

int main(int argc, char *argv[]) {
    int iStack = 222;
    pid_t pid;

    switch(pid = fork()) {
    case -1:
        fprintf(stderr, "Failed to fork() child process - %s\n", strerror(errno));
        break;
    case 0:
        // Child
        iData *= 3;
        iStack *= 3;
        break;
    default:
        // Parent
        sleep(3);
        break;
    }

    printf("PID=%ld (%s) iData=%d iStack=%d\n",
        (long) getpid(), (pid == 0) ? "child" : "parent", iData, iStack);

    return EXIT_SUCCESS;
}
