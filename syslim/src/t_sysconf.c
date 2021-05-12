#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void printSysconf(const char *msg, int name);

int main(int argc, char *argv[]) {
    printSysconf("_SC_ARG_MAX", _SC_ARG_MAX);
    printSysconf("_SC_LOGIN_NAME_MAX", _SC_LOGIN_NAME_MAX);
    printSysconf("_SC_OPEN_MAX", _SC_OPEN_MAX);
    printSysconf("_SC_NGROUPS_MAX", _SC_NGROUPS_MAX);
    printSysconf("_SC_PAGESIZE", _SC_PAGESIZE);
    printSysconf("_SC_RTSIG_MAX", _SC_RTSIG_MAX);

    return 0;
}

void printSysconf(const char *msg, int name) {
    errno = 0;
    long limit = sysconf(name);
    if (limit == -1) {
        if (errno == 0) {
            printf("%s: indeterminate\n", msg); 
        } else {
            fprintf(stderr, "Failed to get limit for %s - %s\n", msg, strerror(errno));
            exit(EXIT_FAILURE);
        }
    } else {
        printf("%s: %ld\n", msg, limit);
    }
}
