#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void printUsage(const char *program);
static long getLong(const char *str, int base);

int main(int argc, char *argv[]) {
    int oflag = 0;

    int opt;
    while((opt = getopt(argc, argv, "cx")) != -1) {
        switch(opt) {
        case 'c':
            oflag |= O_CREAT;
            break;

        case 'x':
            oflag |= O_EXCL;
            break;

        default:
            printUsage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (argc <= optind) {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *semName = argv[optind];
    mode_t mode = (argc <= optind + 1) ? 
        (S_IRUSR | S_IWUSR) : 
        getLong(argv[optind + 1], 8);
    unsigned int value = (argc <= optind + 2) ? 
        0 : getLong(argv[optind + 2], 0);
    if (sem_open(semName, oflag, mode, value) == SEM_FAILED) {
        fprintf(stderr, "Failed to sem_open() %s - %s\n", semName, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    return EXIT_SUCCESS;
}

void printUsage(const char *program) {
    fprintf(stderr, "Usage: %s [-cx] <sem_name> [<octal-perms> [<value>]]\n", program);
    fprintf(stderr, "    -c - Create semaphore (O_CREAT)\n");
    fprintf(stderr, "    -x - Create exclusively (O_EXCL)\n");
}

long getLong(const char *str, int base) {
    char *endptr;
    errno = 0;
    long result = strtol(str, &endptr, base);
    if (errno != 0 && *endptr != '\0') {
        fprintf(stderr, "Failed to convert strtol() %s - %s\n", str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}
