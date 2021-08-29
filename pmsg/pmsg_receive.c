#include <errno.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void printUsage(const char *program);

int main(int argc, char *argv[]) {
    // Parse arguments
    int oflag = O_RDONLY;

    int opt;
    while(getopt(argc, argv, "n") != -1) {
        switch(opt) {
        case 'n':
            oflag |= O_NONBLOCK;
            break;

        default:
            printUsage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (argc <= optind) {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open msg queue
    const char *mqName = argv[optind];
    mqd_t mqd = mq_open(mqName, oflag);
    if (mqd == (mqd_t) -1) {
        fprintf(stderr, "Failed to mq_open() %s - %s\n", mqName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct mq_attr attr;
    if (mq_getattr(mqd, &attr) == -1) {
        fprintf(stderr, "Failed to mq_getattr() %s - %s\n", mqName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Receive msg
    void *buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL) {
        fprintf(stderr, "Failed to malloc() %ld bytes for msg buffer - %s\n", 
            attr.mq_msgsize, strerror(errno));
        exit(EXIT_FAILURE);
    }

    unsigned int priority;
    ssize_t bytesReceived = mq_receive(mqd, buffer, attr.mq_msgsize, &priority);
    if (bytesReceived == -1) {
        fprintf(stderr, "Failed to mq_receive() from %s - %s\n", mqName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Read %ld bytes; priority = %u\n", (long) bytesReceived, priority);
    if (write(STDOUT_FILENO, buffer, bytesReceived) == -1) {
        fprintf(stderr, "Failed to write() %ld bytes - %s\n", bytesReceived, strerror(errno));
        exit(EXIT_FAILURE);
    }
    write(STDOUT_FILENO, "\n", 1);

    // Cleanup
    free(buffer);

    if (mq_close(mqd) == -1) {
        fprintf(stderr, "Failed to mq_close() %s - %s\n", mqName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

void printUsage(const char *program) {
    fprintf(stderr, "Usage: %s [-n] <mq_name>\n", program);
    fprintf(stderr, "    -n - Use O_NONBLOCK flag\n");
}
