#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s <inputFile> <outputFile>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open input and output files
    const char *inputFilepath = argv[1];
    int inputFd = open(inputFilepath, O_RDONLY);
    if (inputFd == -1) {
        fprintf(stderr, "%s: Failed to open file %s\n", strerror(errno), inputFilepath);
        exit(EXIT_FAILURE);
    }

    const char *outputFilepath = argv[2];
    int outputFd = open(outputFilepath,
            O_CREAT | O_WRONLY | O_TRUNC,
            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH); // rw-rw-rw-
    if (outputFd == -1) {
        fprintf(stderr, "%s: Failed to open file %s\n", strerror(errno), outputFilepath);
        exit(EXIT_FAILURE);
    }

    // Transfer data
    ssize_t numRead;
    char buffer[BUFFER_SIZE];
    while ((numRead = read(inputFd, buffer, BUFFER_SIZE)) > 0) {
        if (write(outputFd, buffer, numRead) != numRead) {
            fprintf(stderr, "Failed to write whole buffer to %s\n", outputFilepath);
            exit(EXIT_FAILURE);
        }
    }

    if (numRead == -1) {
        fprintf(stderr, "Failed to read %s\n", inputFilepath);
        exit(EXIT_FAILURE);
    }

    // Cleanup
    if (close(inputFd) == -1) {
        fprintf(stderr, "%s: Failed to close file %s\n", strerror(errno), inputFilepath);
        exit(EXIT_FAILURE);
    }

    if (close(outputFd) == -1) {
        fprintf(stderr, "%s: Failed to close file %s\n", strerror(errno), outputFilepath);
        exit(EXIT_FAILURE);
    }

    return 0;
}
