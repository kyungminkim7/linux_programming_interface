#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>

#include "binary_sems.h"
#include "svshm_xfr.h"

int main(int argc, char *argv[]) {
    // Get semaphores
    int semid = semget(SEM_KEY, 0, 0);    
    if (semid == -1) {
        fprintf(stderr, "Failed to semget() SEM_KEY %x - %s\n", SEM_KEY, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Get shared memory
    int shmid = shmget(SHM_KEY, 0, 0);
    if (shmid == -1) {
        fprintf(stderr, "Failed to shmget() SHM_KEY %x - %s\n", SHM_KEY, strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct shmseg *shmp = shmat(shmid, NULL, SHM_RDONLY);
    if (shmp == (void *) -1) {
        fprintf(stderr, "Failed to shmat() shmid %d - %s\n", shmid, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Transfer data
    int xfrs = 0;
    int bytes = 0;
    for (;; ++xfrs, bytes += shmp->bufferSize) {
        if (reserveSem(semid, READ_SEM) == -1) {
            fprintf(stderr, "Failed to reserve READ_SEM - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        
        if (shmp->bufferSize == 0) {
            break;
        }
        
        if (write(STDOUT_FILENO, shmp->buffer, shmp->bufferSize) != shmp->bufferSize) {
            fprintf(stderr, "Failed to write() - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (releaseSem(semid, WRITE_SEM) == -1) {
            fprintf(stderr, "Failed to release WRITE_SEM - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if (shmdt(shmp) == -1) {
        fprintf(stderr, "Failed to shmdt() shmid %d - %s\n", shmid, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (releaseSem(semid, WRITE_SEM) == -1) {
        fprintf(stderr, "Failed to release WRITE_SEM - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Received %d bytes (%d xfrs)\n", bytes, xfrs);

    return EXIT_SUCCESS;
}
