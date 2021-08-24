#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

#include "semun.h"

static long getLong(const char *str);

int main(int argc, char *argv[]) {
    if (argc < 3 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <semid> <val>...\n", argv[0]);
        exit(EXIT_FAILURE);
    }
        
    int semid = getLong(argv[1]);

    struct semid_ds ds;
    union semun arg;
    arg.buf = &ds;
    if (semctl(semid, 0, IPC_STAT, arg) == -1) {
        fprintf(stderr, "Failed to semctl() IPC_STAT semid %d - %s\n",
            semid, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (argc - 2 != ds.sem_nsems) {
        fprintf(stderr, "Set %d contains %lu semaphores but %d values were supplied.\n",
            semid, ds.sem_nsems, argc - 2);
        exit(EXIT_FAILURE);
    }

    arg.array = calloc(ds.sem_nsems, sizeof(unsigned short));
    if (arg.array == NULL) {
        fprintf(stderr, "Failed to calloc() %lu unsigned shorts - %s\n", ds.sem_nsems, strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (unsigned long i = 0; i < ds.sem_nsems; ++i) {
        arg.array[0] = getLong(argv[i + 2]);
    }

    if (semctl(semid, 0, SETALL, arg) == -1) {
        fprintf(stderr, "Failed to semctl() SET_ALL semid %d - %sn",
            semid, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Semaphore values changed (PID=%ld)\n", (long) getpid());

    return EXIT_SUCCESS;
}

long getLong(const char *str) {
    char *endptr;
    errno = 0;
    long result = strtol(str, &endptr, 0);
    if (errno != 0 && *endptr != '\0') {
        fprintf(stderr, "Failed to convert strtol() %s - %s\n", 
            str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}
