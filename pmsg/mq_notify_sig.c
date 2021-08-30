#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NOTIFY_SIG SIGUSR1

static void handler(int sig) {}

int main(int argc, char *argv[]) {
    if (argc != 2 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <mq_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Initialize msg queue and msg buffer
    const char *mqName = argv[1];
    mqd_t mqd = mq_open(mqName, O_RDONLY | O_NONBLOCK);
    if (mqd == (mqd_t) -1) {
        fprintf(stderr, "Failed to mq_open() %s - %s\n", mqName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct mq_attr attr;
    if (mq_getattr(mqd, &attr) == -1) {
        fprintf(stderr, "Failed to mq_getattr() %s - %s\n", mqName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    void *buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL) {
        fprintf(stderr, "Failed to malloc() %ld bytes for msg buffer - %s\n", attr.mq_msgsize, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Block notification signal
    sigset_t blockMask;
    if (sigemptyset(&blockMask) == -1) {
        fprintf(stderr, "Failed to sigemptyset() block mask - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (sigaddset(&blockMask, NOTIFY_SIG) == -1) {
        fprintf(stderr, "Failed to sigaddset() block mask - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (sigprocmask(SIG_BLOCK, &blockMask, NULL) == -1) {
        fprintf(stderr, "Failed to sigprocmask() block mask - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Set notification signal handler
    struct sigaction act;
    act.sa_handler = handler;
    act.sa_flags = 0;
    if (sigemptyset(&act.sa_mask) == -1) {
        fprintf(stderr, "Failed to sigemptyset() notification handler mask - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (sigaction(NOTIFY_SIG, &act, NULL) == -1) {
        fprintf(stderr, "Failed to set sigaction() for notification signal - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Register for msg availability notification
    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = NOTIFY_SIG;

    if (mq_notify(mqd, &sev) == -1) {
        fprintf(stderr, "mq_notify() failed - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Receive msgs
    sigset_t emptyMask;
    if (sigemptyset(&emptyMask) == -1) {
        fprintf(stderr, "Failed to sigemptyset() suspend mask - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (1) {
        if (sigsuspend(&emptyMask) == -1 && errno != EINTR) {
            fprintf(stderr, "sigsuspend() failed - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (mq_notify(mqd, &sev) == -1) {
            fprintf(stderr, "mq_notify() failed - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        ssize_t bytesReceived;
        while ((bytesReceived = mq_receive(mqd, buffer, attr.mq_msgsize, NULL)) != -1) {
            if (write(STDOUT_FILENO, buffer, bytesReceived) != bytesReceived) {
                fprintf(stderr, "Failed to write %ld bytes - %s\n", (long) bytesReceived, strerror(errno));
                exit(EXIT_FAILURE);
            }
            write(STDOUT_FILENO, "\n", 1);
        }

        if (errno != EAGAIN) {
            fprintf(stderr, "mq_receive() failed - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}
