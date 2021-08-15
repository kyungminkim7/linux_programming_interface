#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

static int getInt(const char *str);
static void printUsage(const char *programName);

int main(int argc, char *argv[]) {
    int msgFlags = 0;
    int opt;
    int numKeyFlags = 0;
    key_t key;
    long lKey;
    while ((opt = getopt(argc, argv, "hcxf:k:p")) != -1) {
        switch (opt) {
        case 'c':
            msgFlags |= IPC_CREAT;
            break;

        case 'x':
            msgFlags |= IPC_EXCL;
            break;

        case 'f':
            key = ftok(optarg, 1);
            if (key == -1) {
                fprintf(stderr, "Failed to ftok() key from file %s - %s\n", optarg, strerror(errno));
                exit(EXIT_FAILURE);
            }
            ++numKeyFlags;
            break;

        case 'k':
            if (sscanf(optarg, "%li", &lKey) != 1) {
                fprintf(stderr, "-k <key> requires a numeric argument\n");
                exit(EXIT_FAILURE);
            }
            ++numKeyFlags;
            break;

        case 'p':
            key = IPC_PRIVATE;
            ++numKeyFlags;
            break;

        case 'h':
        default:
            printUsage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (numKeyFlags != 1) {
        fprintf(stderr, "Exactly one of the options -f, -k, or -p must be supplied\n");
        exit(EXIT_FAILURE);
    }

    int perms = (optind == argc) ? (S_IRUSR | S_IWUSR) : (getInt(argv[optind]));

    int id = msgget(key, msgFlags | perms);
    if (id == -1) {
        fprintf(stderr, "Failed to msgget() msg queue - %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    printf("%d\n", id);
    return EXIT_SUCCESS;
}

static int getInt(const char *str) {
    char *endptr;
    errno = 0;
    int result = strtol(str, &endptr, 8);
    if (errno != 0 && *endptr != '\0') {
        fprintf(stderr, "Failed to strtol() %s - %s\n", str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}

static void printUsage(const char *programName) {
    fprintf(stderr, "Usage: %s [-cx] {-f <pathname> | -k <key> | -p} [<octal perm>]\n", programName);
    fprintf(stderr, "    -c            - Use IPC_CREAT flag\n");
    fprintf(stderr, "    -x            - Use IPC_EXCL flag\n");
    fprintf(stderr, "    -f <pathname> - Generate key using ftok()\n");
    fprintf(stderr, "    -k <key>      - Use key\n");
    fprintf(stderr, "    -p            - Use IPC_PRIVATE key\n");
}
