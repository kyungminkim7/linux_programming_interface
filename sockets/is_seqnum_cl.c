#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "is_seqnum.h"

static long getLong(const char *str);

int main(int argc, char *argv[]) {
    if (argc < 2 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage %s <server> [<seq_length>]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *hostname = argv[1];

    struct addrinfo hint;
    memset(&hint, 0, sizeof(hint));
    hint.ai_flags = AI_NUMERICSERV;
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_addr = NULL;
    hint.ai_canonname = NULL;
    hint.ai_next = NULL;

    struct addrinfo *serverAddrs;
    int status = getaddrinfo(hostname, SERVER_PORT, &hint, &serverAddrs);
    if (status != 0) {
        fprintf(stderr, "Failed to getaddrinfo() for (%s, %s) - %s\n", hostname, SERVER_PORT,
            (status == EAI_SYSTEM) ? strerror(errno) : gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    struct addrinfo *addrp;
    int socketFd;
    for (addrp = serverAddrs; addrp != NULL; addrp = addrp->ai_next) {
        socketFd = socket(addrp->ai_family, addrp->ai_socktype, 0);
        if (socketFd == -1) {
            fprintf(stderr, "Failed to create socket() - %s\n", strerror(errno));
            continue;
        }

        if (connect(socketFd, addrp->ai_addr, addrp->ai_addrlen) != -1) {
            break;
        }
        
        fprintf(stderr, "Failed to connect() - %s\n", strerror(errno));
        if (close(socketFd) == -1) {
            fprintf(stderr, "Failed to close() socket - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if (addrp == NULL) {
        fprintf(stderr, "Failed to locate addr (%s, %s)\n", hostname, SERVER_PORT);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(serverAddrs);

    int request = (argc > 2) ? getLong(argv[2]) : 1;
    if (write(socketFd, &request, sizeof(request)) != sizeof(request)) {
        fprintf(stderr, "Failed to write() to socket - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int response;
    if (read(socketFd, &response, sizeof(response)) == -1) {
        fprintf(stderr, "Failed to read() from socket - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Sequence number: %d\n", response);

    if (close(socketFd) == -1) {
        fprintf(stderr, "Failed to close() socket - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

long getLong(const char *str) {
    char *endptr;
    errno = 0;
    long result = strtol(str, &endptr, 0);
    if (errno != 0 && *endptr != '\0') {
        fprintf(stderr, "Failed to convert strtol() %s - %s\n", str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}
