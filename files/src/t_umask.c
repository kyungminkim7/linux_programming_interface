#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <lpi/FilePerms.h>

int main(int argc, char *argv[]) {
    const char *MY_FILE = "myfile";
    const char *MY_DIR = "mydir";
    const mode_t REQUESTED_FILE_PERMS = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    const mode_t REQUESTED_DIR_PERMS = S_IRWXU | S_IRWXG | S_IRWXO;
    const mode_t PROCESS_UMASK = S_IWGRP | S_IXGRP | S_IWOTH | S_IXOTH;

    umask(PROCESS_UMASK);

    // Create file/dir
    int fd = open(MY_FILE, O_RDWR | O_CREAT | O_EXCL, REQUESTED_FILE_PERMS);
    if (fd == -1) {
        fprintf(stderr, "Failed to open %s - %s\n", MY_FILE, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (mkdir(MY_DIR, REQUESTED_DIR_PERMS) == -1) {
        fprintf(stderr, "Failed to mkdir %s - %s\n", MY_DIR, strerror(errno));
        exit(EXIT_FAILURE);
    }

    const mode_t processUmask = umask(0);

    // Get file/dir stats
    struct stat fileStat;
    if (stat(MY_FILE, &fileStat) == -1) {
        fprintf(stderr, "Failed to stat %s - %s\n", MY_FILE, strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct stat dirStat;
    if (stat(MY_DIR, &dirStat) == -1) {
        fprintf(stderr, "Failed to stat %s - %s\n", MY_DIR, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Print info
    printf("Requested file perms: %s\n", filePermStr(REQUESTED_FILE_PERMS));
    printf("Process umask: %s\n", filePermStr(processUmask));
    printf("Actual file perms: %s\n\n", filePermStr(fileStat.st_mode));

    printf("Requested dir perms: %s\n", filePermStr(REQUESTED_DIR_PERMS));
    printf("Process umask: %s\n", filePermStr(processUmask));
    printf("Actual dir perms: %s\n", filePermStr(dirStat.st_mode));
    
    // Cleanup
    if (unlink(MY_FILE) == -1) {
        fprintf(stderr, "Failed to unlink %s - %s\n", MY_FILE, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (rmdir(MY_DIR) == -1) {
        fprintf(stderr, "Failed to rmdir %s - %s\n", MY_DIR, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return 0;
}
