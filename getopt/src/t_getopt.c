#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "xp:")) != -1) {
        switch (opt) {
        case 'p':
            printf("-p was specified with argument: %s\n", optarg);
            break;
        case 'x':
            printf("-x was specified\n");
            break;
        default:
            exit(EXIT_FAILURE);
        }
    }

    if (optind < argc) {
        printf("Non-option arg: %s\n", argv[optind]);
    }

    return 0;
}
