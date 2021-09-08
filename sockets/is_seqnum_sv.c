#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "is_seqnum.h"

#define BACKLOG 50
#define ADDR_STR_LEN (NI_MAXHOST + NI_MAXSERV + 10)

static long getLong(const char *str);

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <initial_seq_num\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int seqNum = (argc > 1) ? getLong(argv[1]) : 0;

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        fprintf(stderr, "Failed to establish signal() handler for SIGPIPE - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Bind to server port
    struct addrinfo addrHint;
    memset(&addrHint, 0, sizeof(addrHint));
    addrHint.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
    addrHint.ai_family = AF_UNSPEC;
    addrHint.ai_socktype = SOCK_STREAM;
    addrHint.ai_addr = NULL;
    addrHint.ai_canonname = NULL;
    addrHint.ai_next = NULL;

    struct addrinfo *serverAddrs;
    int status = getaddrinfo(NULL, SERVER_PORT, &addrHint, &serverAddrs);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo() failed - %s\n", 
            (status == EAI_SYSTEM) ? strerror(errno) : gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    struct addrinfo *addrp;
    int listenSocketFd;
    for (addrp = serverAddrs; addrp != NULL; addrp = addrp->ai_next) {
        listenSocketFd = socket(addrp->ai_family, addrp->ai_socktype, 0);
        if (listenSocketFd == -1) {
            continue;
        }

        int optval = 1;
        if (setsockopt(listenSocketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
            fprintf(stderr, "Failed to setsockopt() - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (bind(listenSocketFd, addrp->ai_addr, addrp->ai_addrlen) == 0) {
            break;
        }

        if (close(listenSocketFd) == -1) {
            fprintf(stderr, "Failed to close() server listening socket - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if (addrp == NULL) {
        fprintf(stderr, "Failed to bind socket to any address\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(serverAddrs);

    if (listen(listenSocketFd, BACKLOG) == -1) {
        fprintf(stderr, "Failed to listen() on socket - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char clientHostname[NI_MAXHOST];
    char clientSvc[NI_MAXSERV];
    char clientAddrStr[ADDR_STR_LEN];
    while (1) {
        struct sockaddr_storage clientAddr;
        socklen_t clientAddrLength = sizeof(clientAddr);
        int connectedSocketFd = accept(listenSocketFd, (struct sockaddr *) &clientAddr, &clientAddrLength);
        if (connectedSocketFd == -1) {
            fprintf(stderr, "Failed to accept() socket connection - %s\n", strerror(errno));
            continue;
        }
        
        status = getnameinfo((struct sockaddr *) &clientAddr, clientAddrLength, 
            clientHostname, NI_MAXHOST, clientSvc, NI_MAXSERV, 0);
        if (status == 0) {
            snprintf(clientAddrStr, ADDR_STR_LEN, "(%s, %s)", clientHostname, clientSvc);
        } else {
            snprintf(clientAddrStr, ADDR_STR_LEN, "(?UNKNOWN?)");
        }
        printf("Connection from %s\n", clientAddrStr);

        int request;
        if (read(connectedSocketFd, &request, sizeof(request)) == -1) {
            fprintf(stderr, "Failed to read() %ld bytes from socket connection with %s - %s\n", 
                (long) sizeof(request), clientAddrStr, strerror(errno));
            if (close(connectedSocketFd) == -1) {
                fprintf(stderr, "Failed to close() socket connection with %s - %s\n", clientAddrStr, strerror(errno));
                exit(EXIT_FAILURE);
            }
            continue;
        }

        if (write(connectedSocketFd, &seqNum, sizeof(seqNum)) != sizeof(seqNum)) {
            fprintf(stderr, "Failed to write() %ld bytes to socket connection with %s - %s\n", 
                (long) sizeof(seqNum), clientAddrStr, strerror(errno));
        }

        seqNum += request;

        if (close(connectedSocketFd) == -1) {
            fprintf(stderr, "Failed to close() socket connection with %s - %s\n", clientAddrStr, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}

long getLong(const char *str) {
    char *endptr;
    errno = 0;
    const long result = strtol(str, &endptr, 0);
    if (errno != 0 && *endptr != '\0') {
        fprintf(stderr, "Failed to convert strtol() %s - %s\n", str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}
