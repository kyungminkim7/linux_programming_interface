#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <unistd.h>

#define XATTR_SIZE 10000

static void printUsageMsg(const char *program);
static void printValueStr(ssize_t length, const void *value);
static void printValueHex(ssize_t length, const void *value);

int main(int argc, char *argv[]) {
    void (*printXattrValue)(ssize_t length, const void *value) = &printValueStr;

    // Determine xattr print method
    int opt;
    while ((opt = getopt(argc, argv, "x")) != -1) {
        switch(opt) {
        case 'x':
            printXattrValue = &printValueHex;
            break;
        default:
            printUsageMsg(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    char xattrNameList[XATTR_SIZE];
    char xattrValue[XATTR_SIZE];
    for (int i = optind; i < argc; ++i) {
        // Get file xattr list
        const char *filepath = argv[i];
        ssize_t xattrNameListSize = listxattr(filepath, xattrNameList, XATTR_SIZE);
        if (xattrNameListSize == -1) {
            fprintf(stderr, "Failed to get xattr name list of %s - %s\n", 
                filepath, strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Print each xattr name and value in list
        printf("%s:\n", filepath);
        for (char *xattrName = xattrNameList; 
            xattrName < xattrNameList + xattrNameListSize;
            xattrName += strlen(xattrName) + 1) {
            ssize_t xattrValueSize = getxattr(filepath, xattrName, xattrValue, XATTR_SIZE);
            if (xattrValueSize == -1) {
                fprintf(stderr, "Failed to get xattr value for: %s - %s\n", 
                    xattrName, strerror(errno));
            } else {
                printf("\tname=%s; value=", xattrName);
                printXattrValue(xattrValueSize, xattrValue);
            }
        }
    }

    return EXIT_SUCCESS;
}

void printUsageMsg(const char *program) {
    fprintf(stderr, "Usage: %s [-x] <file>...\n", program);
}

void printValueStr(ssize_t length, const void *value) {
    printf("%.*s\n", (int) length, (const char *) value);    
}

void printValueHex(ssize_t length, const void *value) {
    for (const unsigned char *i = value; i < (const unsigned char *)value + length; ++i) {
        printf("%02x ", *i);
    }
    printf("\n");
}
