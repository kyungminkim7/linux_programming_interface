#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <mq_name>\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    const char *mqName = argv[1];
    mqd_t mqd = mq_open(mqName, O_RDONLY);
    if (mqd == -1) {
        fprintf(stderr, "Failed to mq_open() %s - %s\n", mqName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct mq_attr attr;
    if (mq_getattr(mqd, &attr) == -1) {
        fprintf(stderr, "Failed to mq_getattr() %s - %s\n", mqName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Max # of msgs on queue: %ld\n", attr.mq_maxmsg);
    printf("Max msg size:           %ld\n", attr.mq_msgsize);
    printf("# of msgs on queue:     %ld\n", attr.mq_curmsgs);

    if (mq_close(mqd) == -1) {
        fprintf(stderr, "Failed to mq_close() %s - %s\n", mqName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
