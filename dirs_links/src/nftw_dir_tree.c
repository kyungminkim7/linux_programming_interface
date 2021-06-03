#define _XOPEN_SOURCE 600

#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void printUsageError(const char *program);
static int dirTree(const char *pathname, const struct stat *statBuffer, int type, struct FTW *ftw);

int main(int argc, char *argv[]) {
    int flags = 0;
    int opt;
    while ((opt = getopt(argc, argv, "dmp")) != -1) {
        switch (opt) {
        case 'd': flags |= FTW_D; break;        
        case 'm': flags |= FTW_MOUNT; break;        
        case 'p': flags |= FTW_PHYS; break;        
        default:
            printUsageError(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (argc > optind + 1) {
        printUsageError(argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *pathname = (optind > argc ? argv[optind] : ".");
    if (nftw(pathname, dirTree, 10, flags) == -1) {
        fprintf(stderr, "nftw failed\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

void printUsageError(const char *program) {
    fprintf(stderr, "Usage: %s [-d] [-m] [-p] [<file>]\n", program);
}

int dirTree(const char *pathname, const struct stat *statBuffer, int type, struct FTW *ftw) {
    if (type == FTW_NS) {
        printf("?");
    } else {
        switch (statBuffer->st_mode & S_IFMT) {
        case S_IFBLK: printf("b"); break;
        case S_IFCHR: printf("c"); break;
        case S_IFDIR: printf("d"); break;
        case S_IFIFO: printf("p"); break;
        case S_IFLNK: printf("l"); break;
        case S_IFREG: printf("-"); break;
        case S_IFSOCK: printf("s"); break;
        default: printf("?"); break;
        }
    }

    printf(" ");
    switch (type) {
    case FTW_F: printf("F"); break;
    case FTW_D: printf("D"); break;
    case FTW_DNR: printf("DNR"); break;
    case FTW_DP: printf("DP"); break;
    case FTW_NS: printf("NS"); break;
    case FTW_SL: printf("SL"); break;
    case FTW_SLN: printf("SLN"); break;
    default: printf("?"); break;
    }
    printf("   ");

    if (type != FTW_NS) {
        printf("%7ld ", (long) statBuffer->st_ino);
    } else {
        printf("%7s", "");
    }

    printf(" %*s", 4 * ftw->level, ""); // Indent
    printf("%s\n", &pathname[ftw->base]);

    return 0;
}
