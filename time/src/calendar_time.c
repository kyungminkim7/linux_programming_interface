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

    struct tm *gmtmp = gmtime(&time_sec);
    if (gmtmp == NULL) {
        fprintf(stderr, "Failed to convert %ld sec using gmtime()\n", (long) time_sec);
        exit(EXIT_FAILURE);
    }
    struct tm gmtm = *gmtmp;
    printf("Broken down by gmtime():\n");
    printf("  year=%d mon=%d mday=%d hour=%d min=%d sec=%d wday=%d yday=%d isdst=%d\n",
        gmtm.tm_year, gmtm.tm_mon, gmtm.tm_mday, gmtm.tm_hour, gmtm.tm_min, gmtm.tm_sec, 
        gmtm.tm_wday, gmtm.tm_yday, gmtm.tm_isdst);

    struct tm *localtmp = localtime(&time_sec);
    if (localtmp == NULL) {
        fprintf(stderr, "Failed to convert %ld sec using localtime()\n", (long) time_sec);
        exit(EXIT_FAILURE);
    }
    struct tm localtm = *localtmp;
    printf("Broken down by localtime():\n");
    printf("  year=%d mon=%d mday=%d hour=%d min=%d sec=%d wday=%d yday=%d isdst=%d\n\n",
        localtm.tm_year, localtm.tm_mon, localtm.tm_mday, localtm.tm_hour, localtm.tm_min, localtm.tm_sec, 
        localtm.tm_wday, localtm.tm_yday, localtm.tm_isdst);
        
    printf("asctime() formats the gmtime() value as: %s", asctime(&gmtm));
    printf("ctime() formats the time() value as: %s", ctime(&time_sec));

    if ((time_sec = mktime(&gmtm)) == -1) {
        fprintf(stderr, "Failed to mktime\n");
        exit(EXIT_FAILURE);
    }
    printf("mktime() of gmtime() value: %ld secs\n", time_sec);

    if ((time_sec = mktime(&localtm)) == -1) {
        fprintf(stderr, "Failed to mktime\n");
        exit(EXIT_FAILURE);
    }
    printf("mktime() of localtime() value: %ld secs\n", time_sec);

    return 0;
}
