#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 4 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <owner> <group> <file>...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *owner = argv[1];
    uid_t ownerId = -1;
    if (strcmp(owner, "-") != 0) {
        struct passwd *ownerEntry = getpwnam(owner);
        if (ownerEntry == NULL) {
            fprintf(stderr, "Failed to find UID for %s - %s\n", owner, strerror(errno));
            exit(EXIT_FAILURE);
        }
        
        ownerId = ownerEntry->pw_uid;
    }


    const char *owningGroup = argv[2];
    gid_t owningGroupId = -1;
    if (strcmp(owningGroup, "-") != 0) {
        struct group *owningGroupEntry = getgrnam(owningGroup);
        if (owningGroupEntry == NULL) {
            fprintf(stderr, "Failed to find GID for %s - %s\n", owningGroup, strerror(errno));
            exit(EXIT_FAILURE);
        }
        
        owningGroupId = owningGroupEntry->gr_gid;
    }

    for (int i = 3; i < argc; ++i) {
        const char *pathname = argv[i];
        if (chown(pathname, ownerId, owningGroupId) == -1) {
            fprintf(stderr, "Failed to chown %s - %s\n", pathname, strerror(errno));
            exit(EXIT_FAILURE);
        } else {
            printf("Successfully chown'd %s\n", pathname);
        }
    }

    return 0;
}
