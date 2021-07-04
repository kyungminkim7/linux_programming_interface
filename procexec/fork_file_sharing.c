#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char filepath[] = "test_XXXXXX";
    
    setbuf(stdout, NULL);

    int fd = mkstemp(filepath);
    if (fd == -1) {
        fprintf(stderr, "Failed to mkstemp() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("File offset before fork(): %lld\n", (long long) lseek(fd, 0, SEEK_CUR));

    int flags = fcntl(fd, F_GETFD);
    if (flags == -1) {
        fprintf(stderr, "Failed to get file flags from %s - %s\n", filepath, strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("O_APPEND flag before fork() is: %s\n", (flags & O_APPEND) ? "on" : "off");

    switch (fork()) {
    case -1:
        fprintf(stderr, "Failed to fork() child process - %s\n", strerror(errno));
        break;
    case 0:
        // Child process
        if (lseek(fd, 1000, SEEK_SET) == -1) {
            fprintf(stderr, "Failed to lseek() %s - %s\n", filepath, strerror(errno));
            exit(EXIT_FAILURE);
        }

        flags = fcntl(fd, F_GETFD);
        if (flags == -1) {
            fprintf(stderr, "Failed to get file flags from %s - %s\n", filepath, strerror(errno));
            exit(EXIT_FAILURE);
        }

        flags |= O_APPEND;
        if (fcntl(fd, F_SETFD, flags) == -1) {
            fprintf(stderr, "Failed to set O_APPEND flag on fd via fcntl() - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        break;
    default:
        // Parent process
        if (wait(NULL) == -1) {
            fprintf(stderr, "Failed to wait() on child process - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("Child process has exited\n");

        off_t offset = lseek(fd, 0, SEEK_CUR);
        if (offset == -1) {
            fprintf(stderr, "Failed to lseek() %s - %s\n", filepath, strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("File offset in parent: %lld\n", (long long) offset);

        flags = fcntl(fd, F_GETFD);
        if (flags == -1) {
            fprintf(stderr, "Failed to get file flags from %s - %s\n", filepath, strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("O_APPEND flag in parent is: %s\n", (flags & O_APPEND) ? "on" : "off");

        if (unlink(filepath) == -1) {
            fprintf(stderr, "Failed to unlink() %s - %s\n", filepath, strerror(errno));
            break;
        }
        break;
    }

    return EXIT_SUCCESS;
}
