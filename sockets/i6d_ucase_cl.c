#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "i6d_ucase.h"

#define BUFFER_SIZE 10

int main(int argc, char *argv[]) {
    if (argc < 3 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <host_address> <msg>...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const int socketFd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (socketFd == -1) {
        fprintf(stderr, "Failed to create AF_INET6 SOCK_DGRAM socket() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in6 addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(SERVER_PORT);
    const char *addrStr = argv[1];
    if (inet_pton(AF_INET6, addrStr, &addr.sin6_addr) != 1) {
        fprintf(stderr, "inet_pton() failed for address %s - %s\n", addrStr, strerror(errno));
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    for (int i = 2; i < argc; ++i) {
        const ssize_t msgLength = strlen(argv[i]);
        if (sendto(socketFd, argv[i], msgLength, 0, (struct sockaddr *) &addr, sizeof(addr)) != msgLength) {
            fprintf(stderr, "Failed to sendto() (%s, %u) %ld bytes - %s\n", 
                addrStr, SERVER_PORT, (long) msgLength, strerror(errno));
            exit(EXIT_FAILURE);
        }

        const ssize_t numBytes = recvfrom(socketFd, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (numBytes == -1) {
            fprintf(stderr, "Failed to recvfrom() server - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        printf("Response %d: %.*s\n", i - 1, (int) numBytes, buffer);
    }

    if (close(socketFd) == -1) {
        fprintf(stderr, "Failed to close() socket - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
