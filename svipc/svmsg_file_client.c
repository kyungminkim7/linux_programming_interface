#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "svmsg_file.h"

static int clientMsqid;

static void removeClientMsq();

int main(int argc, char *argv[]) {
    // Validate input
    if (argc != 2 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct RequestMsg request;
    if (strlen(argv[1]) > sizeof(request.pathname) - 1) {
        fprintf(stderr, "Pathname is too long (max: %ld bytes)\n", 
            (long) sizeof(request.pathname) - 1);
        exit(EXIT_FAILURE);
    }

    // Get server and client msg queues
    int serverMsqid = msgget(SERVER_KEY, S_IWUSR);
    if (serverMsqid == -1) {
        fprintf(stderr, "Failed to msgget() server msqid - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    clientMsqid = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR | S_IWGRP);
    if (clientMsqid == -1) {
        fprintf(stderr, "Failed to msgget() client msqid - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (atexit(removeClientMsq) == -1) {
        fprintf(stderr, "Failed to register removeClientMsq() with atexit() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Send request
    request.type = 1;
    request.clientId = clientMsqid;
    strncpy(request.pathname, argv[1], sizeof(request.pathname) - 1);
    request.pathname[sizeof(request.pathname) - 1] = '\0';
    if (msgsnd(serverMsqid, &request, REQUEST_MSG_DATA_SIZE, 0) == -1) {
        fprintf(stderr, "Failed to msgsnd() request to server - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Receive response
    struct ResponseMsg response;
    ssize_t bytesReceived = msgrcv(clientMsqid, &response, RESPONSE_MSG_DATA_SIZE, 0, 0);
    if (bytesReceived == -1) {
        fprintf(stderr, "Failed to msgrcv() response from server - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    if (response.type == RESPONSE_MSG_FAILURE) {
        printf("%s\n", response.data);
        exit(EXIT_FAILURE);
    }

    ssize_t totalBytes = bytesReceived;
    int numMsgs = 1;
    for (; response.type != RESPONSE_MSG_END; ++numMsgs) {
        bytesReceived = msgrcv(clientMsqid, &response, RESPONSE_MSG_DATA_SIZE, 0, 0);
        if (bytesReceived == -1) {
            fprintf(stderr, "Failed to msgrcv() response from server - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        
        totalBytes += bytesReceived;
    }

    printf("Received %ld bytes (%d messages)\n", (long) totalBytes, numMsgs);
    return EXIT_SUCCESS;
}

void removeClientMsq() {
    if (msgctl(clientMsqid, IPC_RMID, NULL)) {
        fprintf(stderr, "Failed to remove client msqid %d - %s\n", 
            clientMsqid, strerror(errno));
    }
}
