#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

int main(int argc, char *argv[]) {
    time_t time_sec = time(NULL);
    printf("Seconds since the Epoch (1 Jan 1970): %ld\n", (long) time_sec);

    struct timeval tv;
    if (gettimeofday(&tv, NULL) == -1) {
        fprintf(stderr, "Failed to get time of day - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("  gettimeofday() returned %ld secs, %ld microsecs\n", (long) tv.tv_sec, (long) tv.tv_usec);

    struct tm *gmtm = gmtime(&time_sec);
    printf("Broken down by gmtime():\n");
    printf("  year=%d mon=%d mday=%d hour=%d min=%d sec=%d wday=%d yday=%d isdst=%d\n",
        gmtm->tm_year, gmtm->tm_mon, gmtm->tm_mday, gmtm->tm_hour, gmtm->tm_min, gmtm->tm_sec, 
        gmtm->tm_wday, gmtm->tm_yday, gmtm->tm_isdst);

    return 0;
}
