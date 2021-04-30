#include <setjmp.h>
#include <stdio.h>

static jmp_buf jmpEnv;

static void f1(int argc);
static void f2();

int main(int argc, char *argv[]) {
    switch (setjmp(jmpEnv)) {
    case 0:
        f1(argc);
        break;

    case 1:
        printf("Jumped from f1\n");
        break;

    case 2:
        printf("Jumped from f2\n");
        break;
    }

    return 0;
}

void f1(int argc) {
    if (argc == 1) {
        longjmp(jmpEnv, 1);
    }

    f2();
}

void f2() {
    longjmp(jmpEnv, 2);
}
