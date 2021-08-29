#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static void printUsage(const char *program);
static long getLong(const char *str, int base);

int main(int argc, char *argv[]) {
    int oflag = O_RDWR;

    struct mq_attr *attrp = NULL;
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 2048;

    int opt;
    while ((opt = getopt(argc, argv, "cxm:s:")) != -1) {
        switch(opt) {
        case 'c':
            oflag |= O_CREAT;
            break;

        case 'x':
            oflag |= O_EXCL;
            break;

        case 'm':
            attr.mq_maxmsg = getLong(optarg, 0);
            attrp = &attr;
            break;

        case 's':
            attr.mq_maxmsg = getLong(optarg, 0);
            attrp = &attr;
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

    const char *mq_name = argv[optind];

    mode_t mode = (argc <= optind + 1) ? 
        (S_IRUSR | S_IWUSR) : 
        getLong(argv[optind + 1], 8);

    if (mq_open(mq_name, oflag, mode, attrp) == (mqd_t) -1) {
        fprintf(stderr, "Failed to mq_open() %s - %s\n", mq_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

void printUsage(const char *program) {
    fprintf(stderr, "Usage: %s [-cx] [-m <max_msgs>] [-s <max_msg_size>] <mq_name> [<octal_perms>]\n", program);
    fprintf(stderr, "    -c                - Create queue (O_CREAT)\n");
    fprintf(stderr, "    -x                - Create exclusively (O_EXCL)\n");
    fprintf(stderr, "    -m <max_msgs>     - Set max # of msgs\n");
    fprintf(stderr, "    -s <max_msg_size> - Set max msg size\n");
}

static long getLong(const char *str, int base) {
    char *endptr;
    errno = 0;
    long result = strtol(str, &endptr, base);
    if (errno != 0 && *endptr != '\0') {
        fprintf(stderr, "Failed to convert %s to long - %s\n", str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}
