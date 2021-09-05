#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "us_xfr.h"

#define BUFFER_SIZE 100

int main(int argc, char *argv[]) {
    int socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socketFd == -1) {
        fprintf(stderr, "Failed to create socket() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;

    if (strlen(SV_SOCK_PATH) > sizeof(addr.sun_path) - 1) {
        fprintf(stderr, "Socket path %s is too long (max: %ld)\n", 
            SV_SOCK_PATH, sizeof(addr.sun_path) - 1);
        exit(EXIT_FAILURE);
    }

    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (connect(socketFd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        fprintf(stderr, "Failed to connect() to socket %s - %s\n", SV_SOCK_PATH, strerror(errno));
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    ssize_t numBytes;
    while ((numBytes = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
        if (write(socketFd, buffer, BUFFER_SIZE) != numBytes) {
            fprintf(stderr, "Failed to write() %ld bytes - %s\n", (long) numBytes, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if (numBytes == -1) {
        fprintf(stderr, "Failed to read() from stdin - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (close(socketFd) == -1) {
        fprintf(stderr, "Failed to close() to socket %s - %s\n", SV_SOCK_PATH, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
