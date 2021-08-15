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

int main(int argc, char *argv[]) {
    int opt;
    int flags = 0;
    while ((opt = getopt(argc, argv, "nh")) != -1) {
        switch (opt) {
        case 'n':
            flags |= IPC_NOWAIT;
            break;

        default:
            fprintf(stderr, "Usage: %s [-n] msqid msg-type [msg-text]\n", argv[0]);
            fprintf(stderr, "    -n - Use IPC_NOWAIT flag\n");
            exit(EXIT_FAILURE);
        }
    }

    if (argc < optind + 2 || argc > optind + 3) {
        fprintf(stderr, "Wrong number of arguments\n");
        exit(EXIT_FAILURE);
    }

    int msqid = getLong(argv[optind]);

    struct Msg msg;
    msg.type = getLong(argv[optind + 1]);

    size_t textLength = 0;

    if (argc > optind + 2) {
        const char *text = argv[optind + 2];
        textLength = strlen(text);
        if (textLength > MAX_MSG_TEXT_LENGTH) {
            fprintf(stderr, "msg-text too long (max: %d chars)\n", MAX_MSG_TEXT_LENGTH);
            exit(EXIT_FAILURE);
        }
        memcpy(msg.text, text, textLength);
    }

    if (msgsnd(msqid, &msg, textLength, flags) == -1) {
        fprintf(stderr, "msgsnd() failed - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

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
