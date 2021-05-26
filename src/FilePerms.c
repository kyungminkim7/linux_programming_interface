#include <lpi/FilePerms.h>

#include <stdio.h>
#include <sys/stat.h>

#define FILE_PERM_STR_SIZE sizeof("rwxrwxrwx")

char *filePermStr(mode_t perm) {
    static char str[FILE_PERM_STR_SIZE];    
    snprintf(str, FILE_PERM_STR_SIZE, "%c%c%c%c%c%c%c%c%c",
        perm & S_IRUSR ? 'r' : '-',
        perm & S_IWUSR ? 'w' : '-',
        perm & S_IXUSR ? (perm & S_ISUID ? 's' : 'x') : (perm & S_ISUID ? 'S' : '-'),
        perm & S_IRGRP ? 'r' : '-',
        perm & S_IWGRP ? 'w' : '-',
        perm & S_IXGRP ? (perm & S_ISGID ? 's' : 'x') : (perm & S_ISGID ? 'S' : '-'),
        perm & S_IROTH ? 'r' : '-',
        perm & S_IWOTH ? 'w' : '-',
        perm & S_IXOTH ? (perm & S_ISVTX ? 't' : 'x') : (perm & S_ISVTX ? 'T' : '-'));
    return str;
}
