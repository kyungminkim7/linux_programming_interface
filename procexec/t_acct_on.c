#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc > 2 || (argc > 1 && strcmp(argv[1], "-h") == 0)) {
        fprintf(stderr, "Usage: %s [<file>]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (acct(argv[1]) == -1) {
        fprintf(stderr, "Failed to turn accounting %s - %s\n",
            (argv[1] == NULL) ? "off" : "on", 
            strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Turned accounting %s\n",
        (argv[1] == NULL) ? "off" : "on");

    return EXIT_SUCCESS;
}
