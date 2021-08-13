#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define KEY_PATH "/etc/passwd"

int main(int argc, char *argv[]) {
    key_t key = ftok(KEY_PATH, 1);
    if (key == -1) {
        fprintf(stderr, "Failed to generate ipc key from %s - %s\n", KEY_PATH, strerror(errno));
        exit(EXIT_FAILURE);
    }

    const int MQ_PERM = S_IRUSR | S_IWUSR | S_IWGRP;
    int id;
    while ((id = msgget(key, IPC_CREAT | IPC_EXCL | MQ_PERM)) == -1) {
        if (errno == EEXIST) {
            if ((id = msgget(key, 0)) == -1) {
                fprintf(stderr, "Failed to msgget() old id - %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

            if (msgctl(id, IPC_RMID, NULL) == -1) {
                fprintf(stderr, "Failed to removed old id %d - %s\n", id, strerror(errno));
                exit(EXIT_FAILURE);
            }

            fprintf(stderr, "Removed old id %d\n", id);
        } else {
            fprintf(stderr, "Failed to msgget() id - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    printf("Created new id %d\n", id);
    return EXIT_SUCCESS;
}
