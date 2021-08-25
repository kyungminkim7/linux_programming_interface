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
#include "semun.h"
#include "svshm_xfr.h"

int main(int argc, char *argv[]) {
    // Initialize semaphores
    int semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);
    if (semid == -1) {
        fprintf(stderr, "Failed to semget() SEM_KEY %x - %s\n", SEM_KEY, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (initSemAvailable(semid, WRITE_SEM) == -1) {
        fprintf(stderr, "Failed to initialize WRITE_SEM - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (initSemInUse(semid, READ_SEM) == -1) {
        fprintf(stderr, "Failed to initialize READ_SEM - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Initialize shared memory
    int shmid = shmget(SHM_KEY, sizeof(struct shmseg), IPC_CREAT | OBJ_PERMS);
    if (shmid == -1) {
        fprintf(stderr, "Failed to shmget() SHM_KEY %x - %s\n", SHM_KEY, strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct shmseg *shmp = shmat(shmid, NULL, 0);
    if (shmp == (void *) -1) {
        fprintf(stderr, "Failed to shmat() shmid %d - %s\n", shmid, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Transfer data
    int xfrs = 0;
    int bytes = 0;
    for (shmp->bufferSize = BUF_SIZE; 
        shmp->bufferSize != 0; 
        ++xfrs, bytes += shmp->bufferSize) {

        if (reserveSem(semid, WRITE_SEM) == -1) {
            fprintf(stderr, "Failed to reserve WRITE_SEM %d - %s\n", WRITE_SEM, strerror(errno));
            exit(EXIT_FAILURE);
        }

        shmp->bufferSize = read(STDIN_FILENO, shmp->buffer, BUF_SIZE);
        if (shmp->bufferSize == -1) {
            fprintf(stderr, "Failed to read from stdin - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (releaseSem(semid, READ_SEM) == -1) {
            fprintf(stderr, "Failed to release READ_SEM %d - %s\n", READ_SEM, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    // Check that reader has completed final access to shared memory
    if (reserveSem(semid, WRITE_SEM) == -1) {
        fprintf(stderr, "Failed to reserve WRITE_SEM %d - %s\n", WRITE_SEM, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Cleanup shared memory
    if (shmdt(shmp) == -1) {
        fprintf(stderr, "Failed to shmdt() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        fprintf(stderr, "Failed to shmctl() IPC_RMID semid %d - %s\n", shmid, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Cleanup semaphores
    union semun arg;
    if (semctl(semid, 0, IPC_RMID, arg)) {
        fprintf(stderr, "Failed to semctl() IPC_RMID semid %d - %s\n", semid, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Sent %d bytes (%d xfrs)\n", bytes, xfrs);

    return EXIT_SUCCESS;
}
