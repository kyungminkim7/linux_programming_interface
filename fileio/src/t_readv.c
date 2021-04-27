#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define STRING_LENGTH 100
#define NUM_BUFFERS 3

int main(int argc, char *argv[]) {
    // Validate args
    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open file for reading data
    const char *filepath = argv[1];
    int fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open %s - %s\n", filepath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Data to read
    struct stat fileStat;
    int x;
    char string[STRING_LENGTH];

    // Scatter input data
    struct iovec buffers[NUM_BUFFERS];
    buffers[0].iov_base = &fileStat;
    buffers[0].iov_len = sizeof(fileStat);

    buffers[1].iov_base = &x;
    buffers[1].iov_len = sizeof(x);

    buffers[2].iov_base = &string;
    buffers[2].iov_len = sizeof(string);

    size_t totalSize_bytes = 0;
    for (int i = 0; i < NUM_BUFFERS; ++i) {
        totalSize_bytes += buffers[i].iov_len;
    }

    // Read data
    ssize_t bytesRead = readv(fd, buffers, NUM_BUFFERS);
    if (bytesRead == -1) {
        fprintf(stderr, "Failed to read %s - %s\n", filepath, strerror(errno));
        exit(EXIT_FAILURE);
    } 

    if (bytesRead < totalSize_bytes) {
        printf("Read fewer bytes than requested\n");
    }

    printf("Total bytes requested: %ld; bytes read: %ld\n", 
        (long) totalSize_bytes, (long) bytesRead);

    // Cleanup
    if (close(fd) == -1) {
        fprintf(stderr, "Failed to close %s - %s\n", filepath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return 0;
}
