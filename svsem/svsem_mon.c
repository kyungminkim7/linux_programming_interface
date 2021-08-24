#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>

#include "semun.h"

static int getInt(const char *str);

int main(int argc, char *argv[]) {
    if (argc != 2 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "%s <semid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int semid = getInt(argv[1]);
    
    struct semid_ds ds;
    union semun arg;
    arg.buf = &ds;

    if (semctl(semid, 0, IPC_STAT, arg) == -1) {
        fprintf(stderr, "Failed to semctl() IPC_STAT semid %d - %s\n", semid, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Semaphore changed: %s", ctime(&ds.sem_ctime));
    printf("Last semop():      %s", ctime(&ds.sem_otime));

    arg.array = calloc(ds.sem_nsems, sizeof(unsigned short));
    if (arg.array == NULL) {
        fprintf(stderr, "Failed to calloc() %lu unsigned shorts for sem array values - %s\n", 
            ds.sem_nsems, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (semctl(semid, 0, GETALL) == -1) {
        free(arg.array);
        fprintf(stderr, "Failed to semctl() GETALL semid %d - %s\n", semid, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Sem #  Value  SEMPID  SEMNCNT  SEMZCNT\n");
    union semun dummyArg;
    for (unsigned long i = 0; i < ds.sem_nsems; ++i) {
        printf("%3lu   %5d   %5d   %5d    %5d\n", i, arg.array[i],
            semctl(semid, i, GETPID, dummyArg),
            semctl(semid, i, GETNCNT, dummyArg),
            semctl(semid, i, GETZCNT, dummyArg));
    }

    free(arg.array);

    return EXIT_SUCCESS;
}

int getInt(const char *str) {
    char *endptr;
    errno = 0;
    int result = strtol(str, &endptr, 0);
    if (errno != 0 && *endptr != '\0') {
        fprintf(stderr, "Failed to strtol() %s - %s\n", str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}
