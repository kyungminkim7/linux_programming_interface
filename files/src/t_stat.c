#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysmacros.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

static void printUsageMsg(const char *program);
static void printStat(const struct stat *statBuffer);

int main(int argc, char *argv[]) {
    struct stat statBuffer;
    char *filepath;

    switch(argc) {
    case 2:
        filepath = argv[1];
        if (stat(filepath, &statBuffer) == -1) {
            fprintf(stderr, "Failed to call stat() on %s - %s\n", filepath, strerror(errno));
            exit(EXIT_FAILURE);
        }
        break;

    case 3:
        if (strcmp(argv[1], "-l") != 0) {
            printUsageMsg(argv[0]);
            exit(EXIT_FAILURE);
        }

        filepath = argv[2];
        if (lstat(filepath, &statBuffer) == -1) {
            fprintf(stderr, "Failed to call lstat() on %s - %s\n", filepath, strerror(errno));
            exit(EXIT_FAILURE);
        }
        break;

    default:
        printUsageMsg(argv[0]);
        exit(EXIT_FAILURE);
    }

    printStat(&statBuffer);

    return 0;
}

void printUsageMsg(const char *program) {
    fprintf(stderr, "Usage: %s [-l] <file>\n", program);
}

void printStat(const struct stat *statBuffer) {
    printf("File type: ");
    switch (statBuffer->st_mode & S_IFMT) {
        case S_IFREG: printf("regular file"); break;
        case S_IFDIR: printf("directory"); break;
        case S_IFCHR: printf("character device"); break;
        case S_IFBLK: printf("block device"); break;
        case S_IFIFO: printf("FIFO or pipe"); break;
        case S_IFSOCK: printf("socket"); break;
        case S_IFLNK: printf("symbolic link"); break;
        default: printf("unrecognized file type"); break;
    }

    printf("\nDevice containing i-node: major=%ld minor=%ld\n", 
        (long) major(statBuffer->st_rdev), 
        (long) minor(statBuffer->st_rdev));

    printf("I-node number: %ld\n", (long) statBuffer->st_ino);

    printf("Mode: %lo\n", (unsigned long) statBuffer->st_mode);

    printf("Number of (hard) links: %ld\n", (long) statBuffer->st_nlink);

    printf("Ownership: UID=%ld GID=%ld\n", 
        (long) statBuffer->st_uid, 
        (long) statBuffer->st_gid);

    printf("File size: %ld bytes\n", (long) statBuffer->st_size);

    printf("Optimal I/O block size: %ld bytes\n", (long) statBuffer->st_blksize);
    
    printf("512B blocks allocated: %ld\n", (long) statBuffer->st_blocks);

    printf("Last file access: %s", ctime(&statBuffer->st_atime));
    printf("Last file modification: %s", ctime(&statBuffer->st_mtime));
    printf("Last status change: %s", ctime(&statBuffer->st_ctime));
}

