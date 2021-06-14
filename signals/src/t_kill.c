#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <pid> <sig-num>\n", argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
