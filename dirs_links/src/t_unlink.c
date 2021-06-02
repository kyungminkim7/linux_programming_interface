#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define CMD_SIZE 1024

static unsigned long getUnsignedLong(const char *s);

int main(int argc, char *argv[]) {
    // Parse args
    if (argc == 1 || argc > 3) {
        fprintf(stderr, "Usage: %s <file> [num-1kB-blocks]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filepath = argv[1];
    unsigned long numBlocks = (argc == 3) ? getUnsignedLong(argv[2]) : 100000;

    // Create file and immediately unlink it
    int fd = open(filepath, 
        O_WRONLY | O_CREAT | O_EXCL, 
        S_IRUSR | S_IWUSR);    
    if (fd == -1) {
        fprintf(stderr, "Failed to create file %s - %s\n", filepath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (unlink(filepath) == -1) {
        fprintf(stderr, "Failed to unlink file %s - %s\n", filepath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Fill file with random blocks 
    const char buffer[BUFFER_SIZE];
    for (unsigned long i = 0; i < numBlocks; ++i) {
        if (write(fd, buffer, BUFFER_SIZE) == -1) {
            fprintf(stderr, "Failed to write 1k block to file %s - %s\n",
                filepath, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    // Check disk space usage
    char cmd[CMD_SIZE];
    snprintf(cmd, CMD_SIZE, "df -kT $(dirname %s)", filepath);
    system(cmd);

    // Cleanup
    if (close(fd) == -1) {
        fprintf(stderr, "Failed to close file %s - %s\n", filepath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Check disk space usage
    system(cmd);

    return EXIT_SUCCESS;
}

unsigned long getUnsignedLong(const char *s) {
    char *endptr = NULL;
    errno = 0;
    unsigned long result = strtoul(s, &endptr, 0);
    if (errno != 0 || endptr == s) {
        fprintf(stderr, "Failed to convert %s to unsigned long - %s\n", 
            s, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}
