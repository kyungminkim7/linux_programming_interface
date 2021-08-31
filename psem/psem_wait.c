#include <errno.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

    if (sem_wait(sem) == -1) {
        fprintf(stderr, "Failed to sem_wait() on %s - %s\n", semName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("%ld sem_wait() on %s succeeeded\n", (long) getpid, semName);
    return EXIT_SUCCESS;
}
