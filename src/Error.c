#include <lpi/Error.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum { EXIT_NORMAL, EXIT_IMMEDIATE } ExitType;
static void terminate(ExitType exitType);

static void terminate(ExitType exitType) {
    char *s = getenv("EF_DUMPCORE");
    if (s != NULL && strlen(s) > 0) {
        abort();
    }

    switch (exitType) {
        case EXIT_NORMAL:
            exit(EXIT_FAILURE);
        default:
            _exit(EXIT_FAILURE);
    }
}

