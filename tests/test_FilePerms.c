#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <cmocka.h>

#include <lpi/FilePerms.h>

static void testFilePermsR(void **state);

int main(int argc, char *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(testFilePermsR)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

void testFilePermsR(void **state) {
}
