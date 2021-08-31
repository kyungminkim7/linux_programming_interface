#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

static void printUsage(const char *program);
static unsigned long getUnsignedLong(const char *str, int base);

int main(int argc, char *argv[]) {
    int oflag = O_RDWR;

    int opt;
    while ((opt = getopt(argc, argv, "cx")) != -1) {
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

    const char *shmName = argv[optind];
    mode_t mode = (argc <= optind + 2) ?
        (S_IRUSR | S_IWUSR) :
        getUnsignedLong(argv[optind + 2], 8);
    int shmFd = shm_open(shmName, oflag, mode);
    if (shmFd == -1) {
        fprintf(stderr, "Failed to shm_open() %s - %s\n", shmName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    size_t size = (argc <= optind + 1) ? 0 : getUnsignedLong(argv[optind + 1], 0);
    if (ftruncate(shmFd, size) == -1) {
        fprintf(stderr, "Failed to ftruncate() %s to %lu bytes - %s\n", 
            shmName, (unsigned long) size, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    return EXIT_SUCCESS;
}

void printUsage(const char *program) {
    fprintf(stderr, "Usage: %s [-cx] <shm_name> [<size>] [<octal_perms>]\n", program);
    fprintf(stderr, "    -c - Create shared memory (O_CREAT)\n");
    fprintf(stderr, "    -x - Create exclusively (O_EXCL)\n");
}

unsigned long getUnsignedLong(const char *str, int base) {
    char *endptr;
    errno = 0;
    unsigned long result = strtoul(str, &endptr, base);
    if (errno != 0 && *endptr != '\0') {
        fprintf(stderr, "Failed to convert strtoul() %s - %s\n", str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}
