#include <lpi/Error.h>

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum { EXIT_NORMAL, EXIT_IMMEDIATE } ExitType;
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

static void printErrorMsg(int error, const char *format,  va_list argList) {
    fprintf(stderr, "%s: ",  strerror(error));
    vfprintf(stderr, format, argList);
    fprintf(stderr, "\n");
    fflush(stderr);
}

void errorMsg(const char *format, ...) {
    int savedErrno = errno;

    va_list argList;
    va_start(argList, format);
    printErrorMsg(savedErrno, format, argList);
    va_end(argList);
    fflush(stdout);

    errno = savedErrno;
}

void errorExit(const char *format, ...) {
    va_list argList;
    va_start(argList, format);
    printErrorMsg(errno, format, argList);
    va_end(argList);
    fflush(stdout);

    terminate(EXIT_NORMAL);
}

void errorNumExit(int errorNum, const char *format, ...) {
    va_list argList;
    va_start(argList, format);
    printErrorMsg(errorNum, format, argList);
    va_end(argList);
    fflush(stdout);

    terminate(EXIT_NORMAL);
}

void errorExitImmediate(const char *format, ...) {
    va_list argList;
    va_start(argList, format);
    printErrorMsg(errno, format, argList);
    va_end(argList);
    fflush(stdout);

    terminate(EXIT_IMMEDIATE);
}

void fatal(const char *format, ...) {
    va_list argList;
    va_start(argList, format);
    vfprintf(stderr, format, argList);
    va_end(argList);

    fprintf(stderr, "\n");
    fflush(stderr);
    fflush(stdout);

    terminate(EXIT_NORMAL);
}

void usageError(const char *format, ...) {
    fprintf(stderr, "Usage: ");

    va_list argList;
    va_start(argList, format);
    vfprintf(stderr, format, argList);
    va_end(argList);

    fflush(stderr);
    fflush(stdout);

    exit(EXIT_FAILURE);
}

void cmdlineError(const char *format, ...) {
    fprintf(stderr, "Command-line usage error: ");

    va_list argList;
    va_start(argList, format);
    vfprintf(stderr, format, argList);
    va_end(argList);

    fflush(stderr);
    fflush(stdout);

    exit(EXIT_FAILURE);
}
