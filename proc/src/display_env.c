#include <stdio.h>
#include <unistd.h>

extern char **environ;

int main(int argc, char *argv[]) {
    for (char **p = environ; *p != NULL; ++p) {
        puts(*p);
    }
    return 0;
}
