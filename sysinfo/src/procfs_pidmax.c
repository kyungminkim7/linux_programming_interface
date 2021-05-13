#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 100

int main(int argc, char *argv[]) {
    int fd = open("/proc/sys/kernel/pid_max", O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open file - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_LENGTH];
    ssize_t bytesRead = read(fd, buffer, BUFFER_LENGTH);
    if (bytesRead == -1) {
        fprintf(stderr, "Failed to read file - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("Value: %s\n", buffer);
    

    if (close(fd) == -1) {
        fprintf(stderr, "Failed to close file - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return 0;
}
