#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <unistd.h>

#include "semun.h"

static int getInt(const char *str);

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <init value>\n", argv[0]);
        fprintf(stderr, "       %s <semid> <op>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc == 2) {
        int semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
        if (semid == -1) {
            fprintf(stderr, "semget() failed - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        union semun arg;
        arg.val = getInt(argv[1]);
        if (semctl(semid, 0, SETVAL, arg) == -1) {
            fprintf(stderr, "Failed to initialize semaphore %d - %s\n", 
                semid, strerror(errno));
            exit(EXIT_FAILURE);
        }

        printf("Sempahore ID = %d\n", semid);
    } else {
        int semid = getInt(argv[1]);

        struct sembuf sop;
        sop.sem_num = 0;
        sop.sem_op = (short) getInt(argv[2]);
        sop.sem_flg = 0;

        time_t t = time(NULL);
        printf("%ld: about to semop at %s", (long) getpid(), ctime(&t));
        if (semop(semid, &sop, 1) == -1) {
            fprintf(stderr, "Failed to semop() on semaphore %d with op %d - %s\n", 
                semid, (int) sop.sem_op, strerror(errno));
        }
        printf("%ld: semop completed at %s", (long) getpid(), ctime(&t));
    }

    return EXIT_SUCCESS;
}

int getInt(const char *str) {
    char *endptr;
    errno = 0;
    int result = strtol(str, &endptr, 0);
    if (errno == 0 && *endptr != '\0') {
        fprintf(stderr, "Failed to convert strtol() %s - %s\n", str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}

