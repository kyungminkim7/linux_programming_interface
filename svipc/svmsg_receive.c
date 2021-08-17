#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_MSG_TEXT_LENGTH 1024

struct Msg {
    long type;
    char text[MAX_MSG_TEXT_LENGTH];
};

static long getLong(const char *str);
static void printUsage(const char *program);

int main(int argc, char *argv[]) {
    int opt;
    int flags = 0;
    long type = 0;
    while((opt = getopt(argc, argv, "het:nx")) != -1) {
        switch(opt) {
        case 'e':
            flags |= MSG_NOERROR;
            break;

        case 't':
            type = getLong(optarg);
            break;

        case 'n':
            flags |= IPC_NOWAIT;
            break;

#ifdef MSG_EXCEPT
        case 'x':
            flags |= MSG_EXCEPT;
            break;
#endif

        default:
            printUsage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (argc < optind + 1 || argc > optind + 2) {
        fprintf(stderr, "Wrong number of arguments\n");
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    int msqid = getLong(argv[optind]);
    size_t maxBytes = (argc > optind + 1) ? getLong(argv[optind + 1]) : MAX_MSG_TEXT_LENGTH;

    struct Msg msg;
    ssize_t bytesReceived = msgrcv(msqid, &msg, maxBytes, type, flags);
    if (bytesReceived == -1) {
        fprintf(stderr, "msgrcv() failed - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Received: type=%ld; length=%ld", msg.type, (long) bytesReceived);
    if (bytesReceived > 0) {
        printf("; body=%s", msg.text);
    }
    printf("\n");

    return EXIT_SUCCESS;
}

long getLong(const char *str) {
    char *endptr;
    errno = 0;
    long result = strtol(str, &endptr, 0);
    if (errno != 0 && *endptr != '\0') {
        fprintf(stderr, "Failed to strtol() %s - %s\n", str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}

void printUsage(const char *program) {
    fprintf(stderr, "Usage: %s [<options>] <msqid> [<max-bytes>]\n", program);
    fprintf(stderr, "Permitted options are:\n");
    fprintf(stderr, "    -e        - Use MSG_NOERROR flag\n");
    fprintf(stderr, "    -t <type> - Select msg of given type\n");
    fprintf(stderr, "    -n        - Use IPC_NOWAIT flag\n");
#ifdef MSG_EXCEPT
    fprintf(stderr, "    -x        - Use MSG_EXCEPT flag\n");
#endif
}
