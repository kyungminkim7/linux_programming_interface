#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc == 1 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <filepath>...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; ++i) {
        char *filepath[2];
        for (int j = 0; j < 2; ++j) {
            if ((filepath[j] = strdup(argv[i])) == NULL) {
                fprintf(stderr, "Failed to strdup %s - %s\n", argv[i], strerror(errno));
                exit(EXIT_FAILURE);
            }
        }

        printf("%s ==> %s + %s\n", argv[i], dirname(filepath[0]), basename(filepath[1]));

        for (int j = 0; j < 2; ++j) {
            free(filepath[j]);
        }
    }

    return EXIT_SUCCESS;
}
