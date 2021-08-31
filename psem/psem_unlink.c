#include <errno.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <sem_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(argv[1]) == -1) {
        fprintf(stderr, "Failed to sem_unlink %s - %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
