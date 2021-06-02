#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void listFiles(const char *dirpath);

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s [<dir>]...\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    if (argc == 1) {
        listFiles(".");
    } else {
        for (int i = 1; i < argc; ++i) {
            listFiles(argv[i]);
        }
    }

    return EXIT_SUCCESS;
}

void listFiles(const char *dirpath) {
    DIR *dir = opendir(dirpath);
    if (dir == NULL) {
        fprintf(stderr, "Failed to open dir %s - %s\n", dirpath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    errno = 0;
    for (struct dirent *dirEntry = readdir(dir); 
        dirEntry != NULL; 
        dirEntry = readdir(dir)) {

        if (strcmp(dirEntry->d_name, ".") == 0 || 
            strcmp(dirEntry->d_name, "..") == 0) {
            continue;
        }

        if (strcmp(dirpath, ".") != 0) {
            printf("%s/", dirpath);            
        }
        printf("%s\n", dirEntry->d_name);
    }

    if (errno != 0) {
        fprintf(stderr, "Failed to read dir entry in %s - %s\n", dirpath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (closedir(dir) == -1) {
        fprintf(stderr, "Failed to close dir %s - %s\n", dirpath, strerror(errno));
        exit(EXIT_FAILURE);
    }
}
