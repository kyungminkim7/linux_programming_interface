#include <errno.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "ud_case.h"

#define BUFFER_SIZE 10

int main(int argc, char *argv[]) {
    if (argc < 2 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage %s <msg>...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int socketFd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (socketFd == -1) {
        fprintf(stderr, "Failed to create AF_UNIX SOCK_DGRAM socket() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Bind client socket
    struct sockaddr_un clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sun_family = AF_UNIX;
    snprintf(clientAddr.sun_path, (sizeof(clientAddr.sun_path)), "/tmp/ud_ucase_cl.%ld", (long) getpid());
    if (bind(socketFd, (struct sockaddr *) &clientAddr, sizeof(clientAddr))) {
        fprintf(stderr, "Failed to bind() to socket %s - %s\n", clientAddr.sun_path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Construct server socket address
    struct sockaddr_un serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sun_family = AF_UNIX;
    strncpy(serverAddr.sun_path, SERVER_SOCKET_PATH, sizeof(serverAddr.sun_path) - 1);
    
    char buffer[BUFFER_SIZE];
    for (int i = 1; i < argc; ++i) {
        size_t msgLength = strlen(argv[i]);
        ssize_t bytesSent = sendto(socketFd, argv[i], msgLength, 0, 
            (struct sockaddr *) &serverAddr, sizeof(serverAddr));
        if (bytesSent == -1) {
            fprintf(stderr, "Failed to send() %ld bytes to socket %s - %s\n",
                (long) msgLength, SERVER_SOCKET_PATH, strerror(errno));
            exit(EXIT_FAILURE);
        }

        ssize_t bytesReceived = recvfrom(socketFd, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (bytesReceived == -1) {
            fprintf(stderr, "Failed to recvfrom() socket %s - %s\n", SERVER_SOCKET_PATH, strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("Response: %d: %.*s\n", i, (int) bytesReceived, buffer);
    }

    if (remove(clientAddr.sun_path) == -1) {
        fprintf(stderr, "Failed to remove() socket %s - %s\n", clientAddr.sun_path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (close(socketFd) == -1) {
        fprintf(stderr, "Failed to close() socket - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
