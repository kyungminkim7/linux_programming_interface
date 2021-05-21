#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int opt;
    char *fsType = NULL, *options = NULL;
    unsigned long flags = 0;
    size_t numFlags;
    while ((opt = getopt(argc, argv, "t:o:f:")) != -1) {
        switch (opt) {
        case 't': fsType = optarg; break;
        case 'o': options = optarg; break;
        case 'f':
            numFlags = strlen(optarg);
            for (size_t i = 0; i < numFlags; ++i) {
                switch(optarg[i]) {
                case 'b': flags |= MS_BIND; break;
                case 'd': flags |= MS_DIRSYNC; break;
                case 'l': flags |= MS_MANDLOCK; break;
                case 'm': flags |= MS_MOVE; break;
                case 'A': flags |= MS_NOATIME; break;
                case 'V': flags |= MS_NODEV; break;
                case 'D': flags |= MS_NODIRATIME; break;
                case 'E': flags |= MS_NOEXEC; break;
                case 'S': flags |= MS_NOSUID; break;
                case 'r': flags |= MS_RDONLY; break;
                case 'c': flags |= MS_REC; break;
                case 'R': flags |= MS_REMOUNT; break;
                case 's': flags |= MS_SYNCHRONOUS; break;
                default: 
                    fprintf(stderr, "Unrecognized flag: %c\n", optarg[i]);
                    exit(EXIT_FAILURE);
                }
            }
            break;
        default: exit(EXIT_FAILURE);
        }
    }

    if (argc != (optind + 2)) {
        fprintf(stderr, "Wrong number of arguments provided.\n");
        exit(EXIT_FAILURE);
    }

    const char *source = argv[optind];
    const char *target = argv[optind + 1];

    if (mount(source, target, fsType, flags, options) == -1) {
        fprintf(stderr, "Failed to mount %s on %s - %s\n", 
            source, target, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return 0;
}
