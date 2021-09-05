#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "ud_case.h"

#define BUFFER_SIZE 10

int main(int argc, char *argv[]) {
    int socketFd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (socketFd == -1) {
        fprintf(stderr, "Failed to create AF_UNIX SOCK_DGRAM socket() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sun_family = AF_UNIX;

    if (strlen(SERVER_SOCKET_PATH) > sizeof(serverAddr.sun_path) - 1) {
        fprintf(stderr, "Server socket path %s is too long (max: %ld)\n", 
            SERVER_SOCKET_PATH, (long) serverAddr.sun_path);
        exit(EXIT_FAILURE);
    }
    strncpy(serverAddr.sun_path, SERVER_SOCKET_PATH, sizeof(serverAddr.sun_path) - 1);

    if (remove(SERVER_SOCKET_PATH) == -1 && errno != ENOENT) {
        fprintf(stderr, "Failed to remove() socket %s - %s\n", SERVER_SOCKET_PATH, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (bind(socketFd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
        fprintf(stderr, "Failed to bind() socket to %s - %s\n", 
            SERVER_SOCKET_PATH, strerror(errno));
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    while (1) {
        struct sockaddr_un clientAddr;
        socklen_t clientAddrLength = sizeof(clientAddr);
        ssize_t numBytes = recvfrom(socketFd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &clientAddr, &clientAddrLength);
        if (numBytes == -1) {
            fprintf(stderr, "Failed to recvfrom() socket - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        printf("Server received %ld bytes from %s\n", (long) numBytes, clientAddr.sun_path);

        for (ssize_t i = 0; i < numBytes; ++i) {
            buffer[i] = toupper(buffer[i]);
        }

        if (sendto(socketFd, buffer, numBytes, 0, (struct sockaddr *) &clientAddr, clientAddrLength) != numBytes) {
            fprintf(stderr, "Failed to sendto() %ld bytes - %s\n", numBytes, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}
