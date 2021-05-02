#define _XOPEN_SOURCE

#include <errno.h>
#include <crypt.h>
#include <limits.h>
#include <pwd.h>
#include <shadow.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // Get username
    printf("Username: ");    
    fflush(stdout);

    const long maxUsernameLength = 
        (sysconf(_SC_LOGIN_NAME_MAX) >= 0 ? sysconf(_SC_LOGIN_NAME_MAX) : 256);
    char *username = malloc(maxUsernameLength);
    if (username == NULL) {
        fprintf(stderr, "Failed to allocate %ld bytes for username - %s\n", maxUsernameLength, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (fgets(username, maxUsernameLength, stdin) == NULL) {
        fprintf(stderr, "Failed to get username\n");
        exit(EXIT_FAILURE);
    }
    
    char *lastUsernameChar = &username[strlen(username) - 1];
    if (*lastUsernameChar == '\n') {
        *lastUsernameChar = '\0';
    }

    // Get user's encrypted password entry
    errno = 0;
    struct passwd *passwdEntry = getpwnam(username);
    if (passwdEntry == NULL) {
        if (errno == 0) {
            fprintf(stderr, "Username %s not found\n", username);
        } else {
            fprintf(stderr, "Failed to get passwd entry for %s - %s\n", username, strerror(errno));
        }
        exit(EXIT_FAILURE);
    }

    errno = 0;
    struct spwd *shadowEntry = getspnam(username);
    if (shadowEntry == NULL && errno == EACCES) {
        fprintf(stderr, "No permissions to read shadow file - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char *encryptedPasswordEntry = (shadowEntry != NULL ? shadowEntry->sp_pwdp : passwdEntry->pw_passwd);

    // Get password
    char *password = getpass("Password: ");
    if (password == NULL) {
        fprintf(stderr, "Failed to get passwd - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Validate passwd and erase plaintext password from memory
    char *encryptedPassword = crypt(password, encryptedPasswordEntry);
    for (char *c = password; *c != '\0'; ++c) {
        *c = '\0';
    }

    if (encryptedPassword == NULL) {
        fprintf(stderr, "Failed to encrypt passwd - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (strcmp(encryptedPassword, encryptedPasswordEntry) != 0) {
        puts("Incorrect passwd");
        exit(EXIT_FAILURE);
    }

    printf("Successfully authenticated: UID=%ld\n", (long) passwdEntry->pw_uid);
    
    // Cleanup
    free(username);

    return 0;
}
