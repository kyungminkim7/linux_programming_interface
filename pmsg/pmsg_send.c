#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void printUsage(const char *program);
static unsigned long getUnsignedLong(const char *str);

int main(int argc, char *argv[]) {
    int oflag = O_WRONLY;

    int opt;
    while ((opt = getopt(argc, argv, "n")) != -1) {
        switch (opt) {
        case 'n':
            oflag |= O_NONBLOCK;
            break;

        default:
            printUsage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (argc <= optind + 1) {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *mqName = argv[optind];
    mqd_t mqd = mq_open(mqName, oflag);
    if (mqd == (mqd_t) -1) {
        fprintf(stderr, "Failed to mq_open() %s - %s\n", mqName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    const char *msg = argv[optind + 1];
    unsigned int priority = (argc > optind + 2) ? getUnsignedLong(argv[optind + 2]) : 0;

    if (mq_send(mqd, msg, strlen(msg), priority) == -1) {
        fprintf(stderr, "Failed to mq_send() %s - %s\n", msg, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

void printUsage(const char *program) {
    fprintf(stderr, "Usage: %s [-n] <mq_name> <msg> [<priority>]\n", program);
    fprintf(stderr, "    -n - Use O_NONBLOCK flag\n");
}

unsigned long getUnsignedLong(const char *str) {
    char *endptr;
    errno = 0;
    unsigned long result = strtoul(str, &endptr, 0);
    if (errno != 0 && *endptr != '\0') {
        fprintf(stderr, "Failed to convert strtoul() %s - %s\n", str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}
