#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

int main(int argc, char *argv[]) {
    time_t time_sec = time(NULL);

    char *timeStr = ctime(&time_sec);
    if (timeStr == NULL) {
        
        exit(EXIT_FAILURE);
    }
    printf("ctime() of time() value is: %s\n"

    return 0;
}
