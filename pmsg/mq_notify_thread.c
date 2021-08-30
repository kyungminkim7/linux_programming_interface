#include <errno.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void notifySetup(mqd_t *mqdp);
static void threadFunc(union sigval sv);

int main(int argc, char *argv[]) {
    if (argc != 2 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage %s <mq_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *mqName = argv[1];
    mqd_t mqd = mq_open(mqName, O_RDONLY | O_NONBLOCK);
    if (mqd == (mqd_t) -1) {
        fprintf(stderr, "Failed to mq_open() %s - %s\n", mqName, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    notifySetup(&mqd);
    pause();

    return EXIT_SUCCESS;
}

void notifySetup(mqd_t *mqdp) {
    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_value.sival_ptr = mqdp;
    sev.sigev_notify_function = threadFunc;
    sev.sigev_notify_attributes = NULL;

    if (mq_notify(*mqdp, &sev) == -1) {
        fprintf(stderr, "mq_notify() failed - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

static void threadFunc(union sigval sv) {
    mqd_t *mqdp = sv.sival_ptr;
    struct mq_attr attr;
    if (mq_getattr(*mqdp, &attr) == -1) {
        fprintf(stderr, "mq_getattr() failed - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    void *buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL) {
        fprintf(stderr, "Failed to malloc() %ld bytes - %s\n", attr.mq_msgsize, strerror(errno));
        exit(EXIT_FAILURE);
    }

    notifySetup(mqdp);

    ssize_t bytesReceived;
    while ((bytesReceived = mq_receive(*mqdp, buffer, attr.mq_msgsize, NULL)) != -1) {
        if (write(STDOUT_FILENO, buffer, bytesReceived) != bytesReceived) {
            fprintf(stderr, "Failed to write() %ld bytes - %s\n", (long) bytesReceived, strerror(errno));
            exit(EXIT_FAILURE);
        }
        write(STDOUT_FILENO, "\n", 1);
    }

    if (errno != EAGAIN) {
        fprintf(stderr, "mq_receive() failed - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    free(buffer);
}
