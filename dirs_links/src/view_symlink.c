#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <link>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Check that target file is sym link
    const char *filepath = argv[1];
    struct stat fileStat;
    if (lstat(filepath, &fileStat) == -1) {
        fprintf(stderr, "Failed to get lstat of %s - %s\n", filepath, strerror(errno));
        return EXIT_FAILURE;
    }

    if (!S_ISLNK(fileStat.st_mode)) {
        fprintf(stderr, "%s is not a symbolic link\n", filepath);
        return EXIT_FAILURE;
    }

    // Readlink
    char resolvedFilepath[PATH_MAX];
    ssize_t resolvedFilepathLength = readlink(filepath, resolvedFilepath, PATH_MAX - 1);
    if (resolvedFilepathLength == -1) {
        fprintf(stderr, "Failed to readlink %s - %s\n", filepath, strerror(errno));
        return EXIT_FAILURE;
    }
    resolvedFilepath[resolvedFilepathLength] = '\0';
    printf("readlink: %s --> %s\n", filepath, resolvedFilepath);

    // Realpath
    if (realpath(filepath, resolvedFilepath) == NULL) {
        fprintf(stderr, "Failed to get realpath of %s - %s\n", filepath, strerror(errno));
        return EXIT_FAILURE;
    }
    printf("realpath: %s --> %s\n", filepath, resolvedFilepath);

    return EXIT_SUCCESS;
}
