#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

void displayProcessTimes(const char *msg);
unsigned long getUnsignedLong(const char *str);

int main(int argc, char *argv[]) {
    long ticksPerSec = sysconf(_SC_CLK_TCK);
    if (ticksPerSec == -1) {
        fputs("Failed to get sysconf(_SC_CLK_TCK)", stderr);
        exit(EXIT_FAILURE);
    }

    printf("CLOCKS_PER_SEC=%ld  sysconf(_SC_CLK_TCK)=%ld\n",
        CLOCKS_PER_SEC, (long) ticksPerSec);
    displayProcessTimes("At program start:");

    for (unsigned long i = 0; i < ((argc > 1) ? getUnsignedLong(argv[1]) : 10000000); ++i) {
        getppid();
    }

    displayProcessTimes("After getppid() loop:");

    return 0;
}

void displayProcessTimes(const char *msg) {
    clock_t processTime_tick = clock();
    if (processTime_tick == -1) {
        fprintf(stderr, "Failed to get clock() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct tms processTimesBuffer;
    if (times(&processTimesBuffer) == -1) {
        fprintf(stderr, "Failed to get times() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    long ticksPerSec = sysconf(_SC_CLK_TCK);
    if (ticksPerSec == -1) {
        fputs("Failed to get sysconf(_SC_CLK_TCK)", stderr);
        exit(EXIT_FAILURE);
    }

    puts(msg);
    printf("\tclock() returns: %ld clocks-per-sec (%0.2lf secs)\n", 
        (long) processTime_tick, (double) processTime_tick / CLOCKS_PER_SEC);
    printf("\ttimes() yields: user CPU=%0.2lf; system CPU: %0.2lf\n",
        (double) processTimesBuffer.tms_utime / ticksPerSec,
        (double) processTimesBuffer.tms_stime / ticksPerSec);
}

unsigned long getUnsignedLong(const char *str) {
    char *endptr = NULL;
    errno = 0;
    unsigned long result = strtoul(str, &endptr, 0);
    if (errno != 0 || endptr ==str) {
        fprintf(stderr, "Failed to convert %s to unsigned long\n", str);
        exit(EXIT_FAILURE);
    }
    return result;
}
