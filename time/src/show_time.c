#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define BUFFER_SIZE 256

int main(int argc, char *argv[]) {
    time_t time_sec = time(NULL);

    char *timeStr = ctime(&time_sec);
    if (timeStr == NULL) {
        fprintf(stderr, "Failed to get ctime() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("ctime() of time() value is: %s", timeStr);

    struct tm *tm_local = localtime(&time_sec);
    if (tm_local == NULL) {
        fprintf(stderr, "Failed to get localtime() from time() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if ((timeStr = asctime(tm_local)) == NULL ) {
        fprintf(stderr, "Failed to get asctime() of local time - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("asctime() of local time is: %s", timeStr);

    char buffer[BUFFER_SIZE];
    if (strftime(buffer, BUFFER_SIZE, "%A, %d %B %Y, %H:%M:%S %Z", tm_local) == 0) {
        fprintf(stderr, "Failed to get strftime() of local time - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("strftime() of localtime is %s\n", buffer);

    return 0;
}
