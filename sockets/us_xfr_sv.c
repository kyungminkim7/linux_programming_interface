#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "us_xfr.h"

#define BACKLOG 5
#define BUFFER_SIZE 100

int main(int argc, char *argv[]) {
    // Initialize socket
    int listeningSocketFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listeningSocketFd == -1) {
        fprintf(stderr, "Failed to create UNIX stream socket() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Setup socket address
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    
    if (strlen(SV_SOCK_PATH) > sizeof(addr.sun_path) - 1) {
        fprintf(stderr, "Socket addr %s is too long (max: %ld)\n", SV_SOCK_PATH, (long) sizeof(addr.sun_path));
        exit(EXIT_FAILURE);
    }
    
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    // Remove any previous sockets
    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT) {
        fprintf(stderr, "Failed to remove() %s - %s\n", SV_SOCK_PATH, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Bind listening socket
    if (bind(listeningSocketFd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        fprintf(stderr, "Failed to bind() socket to %s - %s\n", SV_SOCK_PATH, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (listen(listeningSocketFd, BACKLOG) == -1) {
        fprintf(stderr, "Failed to listen() at %s - %s\n", SV_SOCK_PATH, strerror(errno));
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    while (1) {
        int connectedSocketFd = accept(listeningSocketFd, NULL, NULL);
        if (connectedSocketFd == -1) {
            fprintf(stderr, "Failed to accept() socket connection at %s - %s\n", SV_SOCK_PATH, strerror(errno));
            exit(EXIT_FAILURE);
        }

        ssize_t numBytesRead;
        while ((numBytesRead = read(connectedSocketFd, buffer, BUFFER_SIZE)) > 0) {
            if (write(STDOUT_FILENO, buffer, numBytesRead) != numBytesRead) {
                fprintf(stderr, "Failed to write() %ld bytes - %s\n", (long) numBytesRead, strerror(errno));
                exit(EXIT_FAILURE);
            }
        }

        if (numBytesRead == -1) {
            fprintf(stderr, "Failed to read() from socket %s - %s\n", SV_SOCK_PATH, strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (close(connectedSocketFd) == -1) {
            fprintf(stderr, "Failed to close() connected socket %s - %s\n", SV_SOCK_PATH, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}
