#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(int argc, char *argv[]) {
    if (argc == 1 || 
        (argc > 1 && strcmp(argv[1], "-h") == 0)) {
        fprintf(stderr, "Usage: %s <msqid>...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; ++i) {
        int msqid;
        if (sscanf(argv[i], "%i", &msqid) == 0) {
            fprintf(stderr, "msqid must be an integer\n");
            exit(EXIT_FAILURE);
        }

        if (msgctl(msqid, IPC_RMID, NULL) == -1) {
            fprintf(stderr, "Failed to remove msqid %d - %s\n", msqid, strerror(errno));            
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}
