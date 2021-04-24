#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static long getLong(const char *str);

int main(int argc, char *argv[]) {
    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s <file> {r<length>|R<length>|w<string>|s<offset> ...}\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open file for I/O
    const char *filepath = argv[1];
    int fd = open(filepath, 
        O_RDWR | O_CREAT,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH); // rw-rw-rw-
    if (fd == -1) {
        fprintf(stderr, "%s: Failed to open %s\n", strerror(errno), filepath);
        exit(EXIT_FAILURE);
    }

    // Process commands from commandline switches
    for (int i = 2; i < argc; ++i) {
        if (strlen(argv[i]) < 2) {
            fprintf(stderr, "Usage: %s <file> {r<length>|R<length>|w<string>|s<offset> ...}\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        switch(argv[i][0]) {
            case 'r': { // Read bytes and display in text
                long length = getLong(argv[i][1]);
                break;
            }

            case 'R': { // Read bytes and display in hex
                long length = getLong(argv[i][1]);
                break;
            }

            case 'w': {
                break;
            }

            case 's': {
                break;
            }
            default:
                fprintf(stderr, "Command-line usage error - Argument must start with [rRws]: %s\n", argv[i]);
                exit(EXIT_FAILURE);
        }
    }
    
    // Cleanup
    if (close(fd) == -1) {
        fprintf(stderr, "%s: Failed to close %s\n", strerror(errno), filepath);
        exit(EXIT_FAILURE);
    }

    return 0;
}

long getLong(const char *str) {
    errno=0;
    long result = strtol(str, NULL, 10);
    if (errno == -1) {
        fprintf(stderr, "%s: Failed to convert %s to long\n", strerror(errno), str);
        exit(EXIT_FAILURE);
    }

    return result;
}
