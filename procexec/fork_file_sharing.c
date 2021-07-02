#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char filepath[] = "test_XXXXXX";
    
    setbuf(stdout, NULL);

    int fd = mkstemp(filepath);
    if (fd == -1) {
        fprintf(stderr, "Failed to mkstemp() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("File offset before fork(): %lld\n", lseek(fd, 0, SEEK_CUR));

    int flags = fcntl(F_GETFD);
    if (flags == -1) {
        fprintf(stderr, "Failed to get file flags from %s - %s\n", filepath, strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("O_APPEND flag before fork() is: %s", (flags & O_APPEND) ? "on" : "off");

    switch (fork()) {
    case -1:
        fprintf(stderr, "Failed to fork() child process - %s\n", strerror(errno));
        break;
    case 0:
        // Child process
        break;
    default:
        // Parent process
        if (wait(NULL) == -1) {
            fprintf(stderr, "Failed to wait() on child process - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("Child process has exited\n");


        if (unlink(filepath) == -1) {
            fprintf(stderr, "Failed to unlink() %s - %s\n", filepath, strerror(errno));
            break;
        }
        break;
    }


    return EXIT_SUCCESS;
}
