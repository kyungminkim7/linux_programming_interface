#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fifo_seqnum.h"

static char clientFifo[CLIENT_FIFO_NAME_LEN];

static void removeClientFifo();
static int getInt(const char *str);

int main(int argc, char *argv[]) {
    if (argc > 2 || 
        (argc == 2 && strcmp(argv[1], "-h") == 0)) {
        fprintf(stderr, "Usage: %s [<n>]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create client fifo
    umask(0);
    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) getpid());
    if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
        fprintf(stderr, "Failed to mkfifo() %s - %s\n", clientFifo, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (atexit(removeClientFifo) == -1) {
        fprintf(stderr, "Failed to register removeClientFifo() with atexit()\n");
        exit(EXIT_FAILURE);
    }

    // Create and send request to server
    struct Request request;
    request.pid = getpid();
    request.seqLen = (argc == 2 ? getInt(argv[1]) : 1);

    int serverFd = open(SERVER_FIFO, O_WRONLY);
    if (serverFd == -1) {
        fprintf(stderr, "Failed to open() %s for writing - %s\n", SERVER_FIFO, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (write(serverFd, &request, sizeof(request)) != sizeof(request)) {
        fprintf(stderr, "Failed to write() request to %s - %s\n", SERVER_FIFO, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (close(serverFd) == -1) {
        fprintf(stderr, "Failed to close() %s - %s\n", SERVER_FIFO, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Read server response
    int clientFd = open(clientFifo, O_RDONLY);
    if (clientFd == -1) {
        fprintf(stderr, "Failed to open() %s for reading - %s\n", clientFifo, strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct Response response;
    if (read(clientFd, &response, sizeof(response)) != sizeof(response)) {
        fprintf(stderr, "Failed to read() response from %s - %s\n", clientFifo, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("%d\n", response.seqNum);

    if (close(clientFd) == -1) {
        fprintf(stderr, "Failed to close() %s - %s\n", clientFifo, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

void removeClientFifo() {
    if (unlink(clientFifo) == -1) {
        fprintf(stderr, "Failed to unlink() %s - %s\n", clientFifo, strerror(errno));
    }
}

int getInt(const char *str) {
    char *endptr;
    errno = 0;
    int result = strtol(str, &endptr, 0);
    if (errno != 0 && *endptr != '\0') {
        fprintf(stderr, "Failed to convert %s to int - %s\n", str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}
