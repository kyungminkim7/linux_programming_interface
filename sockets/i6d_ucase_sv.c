#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "i6d_ucase.h"

#define BUFFER_SIZE 10

int main(int argc, char *argv[]) {
    const int socketFd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (socketFd == -1) {
        fprintf(stderr, "Failed to create AF_INET6 SOCK_DGRAM socket() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in6 serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin6_family = AF_INET6;
    serverAddr.sin6_port = htons(SERVER_PORT);
    const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
    serverAddr.sin6_addr = in6addr_any;

    if (bind(socketFd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
        fprintf(stderr, "Failed to bind() socket to port %d - %s\n", SERVER_PORT, strerror(errno));
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    char clientAddrStr[INET6_ADDRSTRLEN];
    while (1) {
        struct sockaddr_in6 clientAddr;
        socklen_t clientAddrLength = sizeof(clientAddr);
        ssize_t numBytes = recvfrom(socketFd, buffer, BUFFER_SIZE, 0, 
            (struct sockaddr *) &clientAddr, &clientAddrLength);
        if (numBytes == -1) {
            fprintf(stderr, "Failed to recvfrom() socket port %d - %s\n", SERVER_PORT, strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (inet_ntop(AF_INET6, &clientAddr.sin6_addr, clientAddrStr, INET6_ADDRSTRLEN) == NULL) {
            printf("Failed to convert client address to string - %s\n", strerror(errno));
        } else {
            printf("Server received %ld bytes from (%s, %u)\n", 
                (long) numBytes, clientAddrStr, ntohs(clientAddr.sin6_port));
        }

        for (ssize_t i = 0; i < numBytes; ++i) {
            buffer[i] = toupper(buffer[i]);
        }

        if (sendto(socketFd, buffer, numBytes, 0, (struct sockaddr *) &clientAddr, sizeof(clientAddr)) != numBytes) {
            fprintf(stderr, "Failed to sendto() client %ld bytes - %s\n", (long) numBytes, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}
