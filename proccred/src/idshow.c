#define _GNU_SOURCE

#include <errno.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/fsuid.h>
#include <unistd.h>

char *getUsernameFromUID(uid_t uid);
char *getGroupnameFromGID(gid_t gid);

int main(int argc, char *argv[]) {
    uid_t ruid, euid, suid;
    if (getresuid(&ruid, &euid, &suid) == -1) {
        fprintf(stderr, "Failed to get uids - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    gid_t rgid, egid, sgid;
    if (getresgid(&rgid, &egid, &sgid) == -1) {
        fprintf(stderr, "Failed to get gids - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    uid_t fsuid = setfsuid(0);
    gid_t fsgid = setfsgid(0);

    printf("RUID=%s (%ld); EUID=%s (%ld); SUID=%s (%ld); FSUID=%s (%ld)\n",
        getUsernameFromUID(ruid), (long) ruid,
        getUsernameFromUID(euid), (long) euid,
        getUsernameFromUID(suid), (long) suid,
        getUsernameFromUID(fsuid), (long) fsuid);

    printf("RGID=%s (%ld); EGID=%s (%ld); SGID=%s (%ld); FSGID=%s (%ld)\n",
        getGroupnameFromGID(rgid), (long) rgid,
        getGroupnameFromGID(egid), (long) egid,
        getGroupnameFromGID(sgid), (long) sgid,
        getGroupnameFromGID(fsgid), (long) fsgid);

    gid_t supplementaryGroups[NGROUPS_MAX + 1];
    int numGroups = getgroups(NGROUPS_MAX + 1, supplementaryGroups);
    if (numGroups == -1) {
        fprintf(stderr, "Failed to get supplementary groups - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Supplementary groups (%d):\n", numGroups);
    for (int i = 0; i < numGroups; ++i) {
        printf("%s(%ld)\n", 
            getGroupnameFromGID(supplementaryGroups[i]),
            (long) supplementaryGroups[i]);
    }
    printf("\n");
    
    return 0;
}

char *getUsernameFromUID(uid_t uid) {
    struct passwd *entry = getpwuid(uid);
    if (entry == NULL) {
        fprintf(stderr, "Failed to get user from UID %ld - %s\n", (long) uid, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return entry->pw_name;
}

char *getGroupnameFromGID(gid_t gid) {
    struct group *entry = getgrgid(gid);
    if (entry == NULL) {
        fprintf(stderr, "Failed to get group from GID %ld - %s\n", (long) gid, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return entry->gr_name;
}
