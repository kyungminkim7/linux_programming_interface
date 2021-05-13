#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

int main (int argc, char *argv[]) {
    struct utsname kernelInfo;
    if (uname(&kernelInfo) == -1) {
        fprintf(stderr, "Failed to get uname() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("OS name: %s\n", kernelInfo.sysname);
    printf("Hostname: %s\n", kernelInfo.nodename);
    printf("OS release: %s\n", kernelInfo.release);
    printf("OS version: %s\n", kernelInfo.version);
    printf("Hardware: %s\n", kernelInfo.machine);

    return 0;
}
