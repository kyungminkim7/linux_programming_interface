#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fifo_seqnum.h"

int main(int argc, char *argv[]) {
    umask(0);
    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
        fprintf(stderr, "Failed to mkfifo() at %s - %s\n", SERVER_FIFO, strerror(errno));
        exit(EXIT_FAILURE);
    }

    int serverFd = open(SERVER_FIFO, O_RDONLY);
    if (serverFd == -1) {
        fprintf(stderr, "Failed to open() %s for reading - %s\n", SERVER_FIFO, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (open(SERVER_FIFO, O_WRONLY) == -1) {
        fprintf(stderr, "Failed to open() %s for writing - %s\n", SERVER_FIFO, strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct sigaction act;
    act.sa_handler = SIG_IGN;
    act.sa_flags = 0;
    if (sigaction(SIGPIPE, &act, NULL) == -1) {
        fprintf(stderr, "Failed to set sigaction() of SIGPIPE to SIG_IGN - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (int seqNum = 0;;) {
        // Read request
        struct Request request;
        if (read(serverFd, &request, sizeof(request)) != sizeof(request)) {
            fprintf(stderr, "Failed to read() from %s - %s\n", SERVER_FIFO, strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Open client fifo
        char clientFifo[CLIENT_FIFO_NAME_LEN];
        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) request.pid);
        int clientFd = open(clientFifo, O_WRONLY);
        if (clientFd == -1) {
            fprintf(stderr, "Failed to open() %s for writing - %s\n", clientFifo, strerror(errno));
            continue;
        }

        // Write client response
        struct Response response;
        response.seqNum = seqNum;
        if (write(clientFd, &response, sizeof(response)) != sizeof(response)) {
            fprintf(stderr, "Failed to write() response to %s - %s\n", clientFifo, strerror(errno));
            continue;
        }

        if (close(clientFd) == -1) {
            fprintf(stderr, "Failed to close() %s - %s\n", clientFifo, strerror(errno));
        }

        seqNum += request.seqLen;
    }

    return EXIT_SUCCESS;
}
