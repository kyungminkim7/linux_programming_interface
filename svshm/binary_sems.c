#include "binary_sems.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "semun.h"

bool bsUseSemUndo = false;
bool bsRetryOnEintr = true;

int initSemAvailable(int semId, int semNum) {
    union semun arg;
    arg.val = 1;
    return semctl(semId, semNum, SETVAL, arg);
}

int initSemInUse(int semId, int semNum) {
    union semun arg;
    arg.val = 0;
    return semctl(semId, semNum, SETVAL, arg);
}

int reserveSem(int semId, int semNum) {
    struct sembuf sop;
    sop.sem_num = semNum;
    sop.sem_op = -1;
    sop.sem_flg = bsUseSemUndo ? SEM_UNDO : 0;

    while (semop(semId, &sop, 1) == -1) {
        if (errno != EINTR || !bsRetryOnEintr) {
            return -1;
        }
    }

    return 0;
}

int releaseSem(int semId, int semNum) {
    struct sembuf sop;
    sop.sem_num = semNum;
    sop.sem_op = 1;
    sop.sem_flg = bsUseSemUndo ? SEM_UNDO : 0;
    return semop(semId, &sop, 1);
}
