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

    const char *semName = argv[1];
    sem_t *sem = sem_open(semName, 0);
    if (sem == SEM_FAILED) {
        fprintf(stderr, "Failed to sem_open() %s - %s\n", semName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (sem_post(sem) == -1) {
        fprintf(stderr, "Failed to sem_post() to %s - %s\n", semName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
