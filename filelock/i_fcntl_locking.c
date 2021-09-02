#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE 100

static void printCmdFormat();

int main(int argc, char *argv[]) {
    if (argc != 2 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *pathname = argv[1];
    int fd = open(pathname, O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "Failed to open() %s - %s\n", pathname, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Enter ? for help\n");

    char line[MAX_LINE];
    while (1) {
        printf("PID=%ld> ", (long) getpid());
        fflush(stdout);

        if (fgets(line, MAX_LINE, stdin) == NULL) { break; } // EOF
        line[strlen(line) - 1] = '\0'; // Remove trailing newline
        if (line[0] == '\0') { continue; } // Ignore blank lines

        if (line[0] == '?') {
            printCmdFormat();
            continue;
        }
        
        char whence = 's';
        char cmdCh, lock;
        long long start, length;
        if (sscanf(line, "%c %c %lld %lld %c", 
                &cmdCh, &lock, &start, &length, &whence) < 4 ||
            strchr("gsw", cmdCh) == NULL ||
            strchr("rwu", lock) == NULL ||
            strchr("sce", whence) == NULL) {
            printf("Invalid command!\n");
            continue;
        }
        
        int cmd;
        switch (cmdCh) {
        case 'g': cmd = F_GETLK; break;
        case 's': cmd = F_SETLK; break;
        case 'w': cmd = F_SETLKW; break;
        }

        struct flock fl;

        switch (lock) {
        case 'r': fl.l_type = F_RDLCK; break;
        case 'w': fl.l_type = F_WRLCK; break;
        case 'u': fl.l_type = F_UNLCK; break;
        }

        switch (whence) {
        case 's': fl.l_whence = SEEK_SET; break;
        case 'c': fl.l_whence = SEEK_CUR; break;
        case 'e': fl.l_whence = SEEK_END; break;
        }

        fl.l_start = start;
        fl.l_len = length;

        int status = fcntl(fd, cmd, &fl);

        if (cmd == F_GETLK) {
            if (status == -1) {
                fprintf(stderr, "Failed to fcntl() F_GETLK - %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
                
            if (fl.l_type == F_UNLCK) {
                printf("[PID=%ld] Lock can be placed\n", (long) getpid());
            } else {
                printf("[PID=%ld] Denied by %s lock on %lld:%lld (held by PID %ld)\n",
                    (long) getpid(),
                    (fl.l_type == F_RDLCK) ? "READ" : "WRITE",
                    (long long) fl.l_start, (long long) fl.l_len,
                    (long) fl.l_pid);
            }
        } else {
            if (status == 0) {
                printf("[PID=%ld] %s\n", (long) getpid(),
                    (fl.l_type == F_UNLCK) ? "unlocked" : "got lock");
            } else if (errno == EAGAIN || errno == EACCES) {
                printf("[PID=%ld] failed (incompatible lock)\n", (long) getpid());
            } else if (errno == EDEADLK) {
                printf("[PID=%ld] failed (deadlock)\n", (long) getpid());
            } else {
                fprintf(stderr, "Failed to fcntl() F_SETLK(W) - %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    }

    return EXIT_SUCCESS;
}

void printCmdFormat() {
    printf("\n    Format: <cmd> <lock> <start> <length> [<whence>]\n");
    printf("    <cmd>    - g (GETLK) | s (SETLK) | w (SETLKW)\n");
    printf("    <lock>   - r (READ) | w (WRITE) | u (UNLOCK)\n");
    printf("    <whence> - s (SEEK_SET, default) | c (SEEK_CUR) | e (SEEK_END)\n\n");
}
