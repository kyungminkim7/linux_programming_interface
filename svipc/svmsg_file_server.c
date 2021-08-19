#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>

#include "svmsg_file.h"

static void childCleanupHandler(int sig);
static void serveRequest(const struct RequestMsg *request);

int main(int argc, char *argv[]) {
    const int msqPerms = S_IRUSR | S_IWUSR | S_IWGRP;
    int msqid;
    while ((msqid = msgget(SERVER_KEY, IPC_CREAT | IPC_EXCL | msqPerms)) == -1) {
        if (errno == EEXIST) {
            if ((msqid = msgget(SERVER_KEY, msqPerms)) == -1) {
                fprintf(stderr, "Failed to msgget() server id from key 0x%x - %s\n", SERVER_KEY, strerror(errno));
                exit(EXIT_FAILURE);
            }
            
            break;
        } else {
            fprintf(stderr, "Failed to msgget() server id from key 0x%x - %s\n", SERVER_KEY, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    // Establish child process cleanup handler
    struct sigaction act;
    act.sa_handler = childCleanupHandler;
    if(sigemptyset(&act.sa_mask) == -1) {
        fprintf(stderr, "Failed to sigemptyset() sigaction mask - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    act.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &act, NULL) == -1) {
        fprintf(stderr, "Failed to set sigaction() for SIGCHLD - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    while (1) {
        struct RequestMsg request;
        ssize_t bytesReceived = msgrcv(msqid, &request, REQUEST_MSG_DATA_SIZE, 0, 0);
        if (bytesReceived == -1) {
            if (errno == EINTR) {
                continue;
            }

            fprintf(stderr, "msgrcv() failed on msqid %d - %s\n", msqid, strerror(errno));
            break;
        }

        pid_t pid = fork();
        if (pid == -1) {
            fprintf(stderr, "Failed to fork() - %s\n", strerror(errno));
            break;
        }

        if (pid == 0) {
            serveRequest(&request);
            _exit(EXIT_SUCCESS);
        }
    }

    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        fprintf(stderr, "Failed to remove msqid %d - %s\n", msqid, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

void childCleanupHandler(int sig) {
    int savedErrno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = savedErrno;
}

void serveRequest(const struct RequestMsg *request) {
    struct ResponseMsg response;

    int fd = open(request->pathname, O_RDONLY);
    if (fd == -1) {
        response.type = RESPONSE_MSG_FAILURE;
        snprintf(response.data, sizeof(response.data), "Failed to open %s for reading - %s\n", 
            request->pathname, strerror(errno));

        if (msgsnd(request->clientId, &response, strlen(response.data) + 1, 0) == -1) {
            fprintf(stderr, "Failed to msgsnd() RESPONSE_MSG_FAILURE to %d - %s\n", request->clientId, strerror(errno));
        }
        exit(EXIT_FAILURE);
    }

    ssize_t bytesRead;
    while ((bytesRead = read(fd, &response.data, RESPONSE_MSG_DATA_SIZE)) > 0) {
        response.type = RESPONSE_MSG_DATA;
        if (msgsnd(request->clientId, &response, bytesRead, 0) == -1) {
            fprintf(stderr, "Failed to msgsnd() RESPONSE_MSG_DATA to %d - %s\n", request->clientId, strerror(errno));
            break;
        }
    }

    if (bytesRead == -1) {
        fprintf(stderr, "Failed to read() %s - %s\n", request->pathname, strerror(errno));
    }

    response.type = RESPONSE_MSG_END;
    if (msgsnd(request->clientId, &response, 0, 0) == -1) {
        fprintf(stderr, "Failed to msgsnd() RESPONSE_MSG_END to %d - %s\n", request->clientId, strerror(errno));
    }

    if (close(fd) == -1) {
        fprintf(stderr, "Failed to close %s - %s\n", request->pathname, strerror(errno));
        exit(EXIT_FAILURE);
    }
}
