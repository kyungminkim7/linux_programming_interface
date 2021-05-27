#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <cmocka.h>

#include <sys/stat.h>

#include <lpi/FilePerms.h>

static void testFilePermsNone(void **state);
static void testFilePermsR(void **state);
static void testFilePermsW(void **state);
static void testFilePermsX(void **state);
static void testFilePermsRWX(void **state);

int main(int argc, char *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(testFilePermsNone),
        cmocka_unit_test(testFilePermsR),
        cmocka_unit_test(testFilePermsW),
        cmocka_unit_test(testFilePermsX),
        cmocka_unit_test(testFilePermsRWX)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

void testFilePermsNone(void **state) {
    assert_string_equal(filePermStr(0), "---------");
    assert_string_equal(filePermStr(S_ISUID), "--S------"); // SUID
    assert_string_equal(filePermStr(S_ISGID), "-----S---"); // SGID
    assert_string_equal(filePermStr(S_ISVTX), "--------T"); // Sticky bit
}

void testFilePermsR(void **state) {
    assert_string_equal(filePermStr(S_IRUSR), "r--------");
    assert_string_equal(filePermStr(S_IRGRP), "---r-----");
    assert_string_equal(filePermStr(S_IROTH), "------r--");

    assert_string_equal(filePermStr(S_IRUSR | S_IRGRP), "r--r-----");
    assert_string_equal(filePermStr(S_IRUSR | S_IROTH), "r-----r--");
    assert_string_equal(filePermStr(S_IRGRP | S_IROTH), "---r--r--");

    assert_string_equal(filePermStr(S_IRUSR | S_IRGRP | S_IROTH), "r--r--r--");
}

void testFilePermsW(void **state) {
    assert_string_equal(filePermStr(S_IWUSR), "-w-------");
    assert_string_equal(filePermStr(S_IWGRP), "----w----");
    assert_string_equal(filePermStr(S_IWOTH), "-------w-");

    assert_string_equal(filePermStr(S_IWUSR | S_IWGRP), "-w--w----");
    assert_string_equal(filePermStr(S_IWUSR | S_IWOTH), "-w-----w-");
    assert_string_equal(filePermStr(S_IWGRP | S_IWOTH), "----w--w-");

    assert_string_equal(filePermStr(S_IWUSR | S_IWGRP | S_IWOTH), "-w--w--w-");
}

void testFilePermsX(void **state) {
    assert_string_equal(filePermStr(S_IXUSR), "--x------");
    assert_string_equal(filePermStr(S_IXGRP), "-----x---");
    assert_string_equal(filePermStr(S_IXOTH), "--------x");

    assert_string_equal(filePermStr(S_IXUSR | S_IXGRP), "--x--x---");
    assert_string_equal(filePermStr(S_IXUSR | S_IXOTH), "--x-----x");
    assert_string_equal(filePermStr(S_IXGRP | S_IXOTH), "-----x--x");

    assert_string_equal(filePermStr(S_IXUSR | S_IXGRP | S_IXOTH), "--x--x--x");

    // SUID
    assert_string_equal(filePermStr(S_ISUID | S_IXUSR), "--s------");
    assert_string_equal(filePermStr(S_ISUID | S_IXGRP), "--S--x---");
    assert_string_equal(filePermStr(S_ISUID | S_IXOTH), "--S-----x");

    assert_string_equal(filePermStr(S_ISUID | S_IXUSR | S_IXGRP), "--s--x---");
    assert_string_equal(filePermStr(S_ISUID | S_IXUSR | S_IXOTH), "--s-----x");
    assert_string_equal(filePermStr(S_ISUID | S_IXGRP | S_IXOTH), "--S--x--x");

    assert_string_equal(filePermStr(S_ISUID | S_IXUSR | S_IXGRP | S_IXOTH), "--s--x--x");

    // SGID
    assert_string_equal(filePermStr(S_ISGID | S_IXUSR), "--x--S---");
    assert_string_equal(filePermStr(S_ISGID | S_IXGRP), "-----s---");
    assert_string_equal(filePermStr(S_ISGID | S_IXOTH), "-----S--x");

    assert_string_equal(filePermStr(S_ISGID | S_IXUSR | S_IXGRP), "--x--s---");
    assert_string_equal(filePermStr(S_ISGID | S_IXUSR | S_IXOTH), "--x--S--x");
    assert_string_equal(filePermStr(S_ISGID | S_IXGRP | S_IXOTH), "-----s--x");

    assert_string_equal(filePermStr(S_ISGID | S_IXUSR | S_IXGRP | S_IXOTH), "--x--s--x");
    
    // Sticky bit
    assert_string_equal(filePermStr(S_ISVTX | S_IXUSR), "--x-----T");
    assert_string_equal(filePermStr(S_ISVTX | S_IXGRP), "-----x--T");
    assert_string_equal(filePermStr(S_ISVTX | S_IXOTH), "--------t");

    assert_string_equal(filePermStr(S_ISVTX | S_IXUSR | S_IXGRP), "--x--x--T");
    assert_string_equal(filePermStr(S_ISVTX | S_IXUSR | S_IXOTH), "--x-----t");
    assert_string_equal(filePermStr(S_ISVTX | S_IXGRP | S_IXOTH), "-----x--t");

    assert_string_equal(filePermStr(S_ISVTX | S_IXUSR | S_IXGRP | S_IXOTH), "--x--x--t");
}

void testFilePermsRWX(void **state) {
    assert_string_equal(filePermStr(
        S_IRUSR | S_IWUSR | S_IXUSR),
        "rwx------");
    assert_string_equal(filePermStr(
        S_IRGRP | S_IWGRP | S_IXGRP),
        "---rwx---");
    assert_string_equal(filePermStr(
        S_IROTH | S_IWOTH | S_IXOTH),
        "------rwx");
    assert_string_equal(filePermStr(
        S_IRUSR | S_IWUSR | S_IXUSR |
        S_IRGRP | S_IWGRP | S_IXGRP),
        "rwxrwx---");
    assert_string_equal(filePermStr(
        S_IRUSR | S_IWUSR | S_IXUSR |
        S_IROTH | S_IWOTH | S_IXOTH), 
        "rwx---rwx");
    assert_string_equal(filePermStr(
        S_IRGRP | S_IWGRP | S_IXGRP |
        S_IROTH | S_IWOTH | S_IXOTH), 
        "---rwxrwx");
    assert_string_equal(filePermStr(
        S_IRUSR | S_IWUSR | S_IXUSR |
        S_IRGRP | S_IWGRP | S_IXGRP |
        S_IROTH | S_IWOTH | S_IXOTH), 
        "rwxrwxrwx");
    
    // SUID, SGID, Sticky Bit
    assert_string_equal(filePermStr(
        S_ISUID | S_ISGID | S_ISVTX |
        S_IRUSR | S_IWUSR | S_IXUSR),
        "rws--S--T");
    assert_string_equal(filePermStr(
        S_ISUID | S_ISGID | S_ISVTX |
        S_IRGRP | S_IWGRP | S_IXGRP),
        "--Srws--T");
    assert_string_equal(filePermStr(
        S_ISUID | S_ISGID | S_ISVTX |
        S_IROTH | S_IWOTH | S_IXOTH),
        "--S--Srwt");
    assert_string_equal(filePermStr(
        S_ISUID | S_ISGID | S_ISVTX |
        S_IRUSR | S_IWUSR | S_IXUSR |
        S_IRGRP | S_IWGRP | S_IXGRP),
        "rwsrws--T");
    assert_string_equal(filePermStr(
        S_ISUID | S_ISGID | S_ISVTX |
        S_IRUSR | S_IWUSR | S_IXUSR |
        S_IROTH | S_IWOTH | S_IXOTH), 
        "rws--Srwt");
    assert_string_equal(filePermStr(
        S_ISUID | S_ISGID | S_ISVTX |
        S_IRGRP | S_IWGRP | S_IXGRP |
        S_IROTH | S_IWOTH | S_IXOTH), 
        "--Srwsrwt");
    assert_string_equal(filePermStr(
        S_ISUID | S_ISGID | S_ISVTX |
        S_IRUSR | S_IWUSR | S_IXUSR |
        S_IRGRP | S_IWGRP | S_IXGRP |
        S_IROTH | S_IWOTH | S_IXOTH), 
        "rwsrwsrwt");
}
