#include <errno.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <mq_name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *name = argv[1];
    if (mq_unlink(name) == -1) {
        fprintf(stderr, "Failed to mq_unlink() %s - %s\n", name, strerror(errno));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
