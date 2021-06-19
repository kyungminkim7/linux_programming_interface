#include <crypt.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static long handled = 0;
static char *str2;

static void sigHandler(int sig);

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <str1> <str2>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *str1 = argv[1];
    str2 = argv[2];

    // Encrypt 1st arg and save for checking later
    const char *expectedHash = strdup(crypt(str1, "xx"));
    if (expectedHash == NULL) {
        fprintf(stderr, "Failed to strdup() %s - %s\n", str1, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Establish signal handler
    struct sigaction action;
    action.sa_handler = sigHandler;
    if (sigemptyset(&action.sa_mask) == -1) {
        fprintf(stderr, "Failed to sigemtpyset() signal mask - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    action.sa_flags = 0;
    if(sigaction(SIGINT, &action, NULL) == -1) {
        fprintf(stderr, "Failed to set sigaction() for INTQUIT - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    long numMismatch = 0;
    for (long i = 0; ; ++i) {
        if(strcmp(crypt(str1, "xx"), expectedHash) != 0) {
            printf("Mismatch on call %ld (mismatch=%ld handled=%ld)\n",
                i, ++numMismatch, handled);
        }
    }

    return EXIT_SUCCESS;
}

void sigHandler(int sig) {
    crypt(str2, "xx");
    ++handled;
}
