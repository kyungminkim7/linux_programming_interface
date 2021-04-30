#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

typedef enum { TEXT,HEX } DisplayType;

unsigned long getUnsignedLong(const char *str);
long getLong(const char *str);
void readDisplayBytes(int fd, unsigned long length, DisplayType displayType);

int main(int argc, char *argv[]) {
    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s <file> {r<length>|R<length>|w<string>|s<offset> ...}\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open file for I/O
    const char *filepath = argv[1];
    int fd = open(filepath, 
        O_RDWR | O_CREAT,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH); // rw-rw-rw-
    if (fd == -1) {
        fprintf(stderr, "%s: Failed to open %s\n", strerror(errno), filepath);
        exit(EXIT_FAILURE);
    }

    // Process commands from commandline switches
    for (int i = 2; i < argc; ++i) {
        if (strlen(argv[i]) < 2) {
            fprintf(stderr, "Usage: %s <file> {r<length>|R<length>|w<string>|s<offset> ...}\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        switch(argv[i][0]) {
            case 'r': { // Read bytes and display in text at current offset
                unsigned long length = getUnsignedLong(&argv[i][1]);
                readDisplayBytes(fd, length, TEXT);
                break;
            }

            case 'R': { // Read bytes and display in hex at current offset
                unsigned long length = getUnsignedLong(&argv[i][1]);
                readDisplayBytes(fd, length, HEX);
                break;
            }

            case 'w': { // Write string at current offset
                const char *str = &argv[i][1];
                ssize_t numBytesWritten = write(fd, str, strlen(str));
                if (numBytesWritten == -1) {
                    fprintf(stderr, "%s: Failed to write to %s\n", strerror(errno), filepath);
                    exit(EXIT_FAILURE);
                }
                printf("Wrote %lu bytes to %s\n", (unsigned long) numBytesWritten, filepath);
                break;
            }

            case 's': { // Seek to byte offset from start of file
                long offset = getLong(&argv[i][1]);
                off_t offsetSeeked = lseek(fd, offset, SEEK_SET);
                if (offsetSeeked != offset) {
                    fprintf(stderr, "Failed to seek to offset %ld\n", (long) offset);
                    exit(EXIT_FAILURE);
                }
                printf("Seeked to offset %ld\n", (long) offsetSeeked);
                break;
            }
            default:
                fprintf(stderr, "Command-line usage error - Argument must start with [rRws]: %s\n", argv[i]);
                exit(EXIT_FAILURE);
        }
    }
    
    // Cleanup
    if (close(fd) == -1) {
        fprintf(stderr, "%s: Failed to close %s\n", strerror(errno), filepath);
        exit(EXIT_FAILURE);
    }

    return 0;
}

unsigned long getUnsignedLong(const char *str) {
    errno = 0;
    unsigned long result = strtoul(str, NULL, 10);
    if (errno != 0) {
        fprintf(stderr, "%s: Failed to convert %s to unsigned long\n", strerror(errno), str);
        exit(EXIT_FAILURE);
    }
    return result;
}

long getLong(const char *str) {
    errno = 0;
    long result = strtol(str, NULL, 10);
    if (errno != 0) {
        fprintf(stderr, "%s: Failed to convert %s to long\n", strerror(errno), str);
        exit(EXIT_FAILURE);
    }
    return result;
}

void readDisplayBytes(int fd, unsigned long length, DisplayType displayType) {
    char *buffer = malloc(length); 
    if (buffer == NULL) {
        fprintf(stderr, "Failed to malloc %ld bytes\n", length);
        exit(EXIT_FAILURE);
    }
    ssize_t numBytesRead = read(fd, buffer, length);

    if (numBytesRead == -1) {
        fprintf(stderr, "%s: Failed to read %ld bytes\n", strerror(errno), length);
        exit(EXIT_FAILURE);
    } else if (numBytesRead == 0) {
        printf("End-of-file\n");
    } else {
        switch (displayType) {
        case TEXT:
            for (int j = 0; j < numBytesRead; ++j) {
                printf("%c", isprint((unsigned char) buffer[j]) ? buffer[j] : '?');
            }
            break;
        case HEX:
            for (int j = 0; j < numBytesRead; ++j) {
                printf("%02x ", (unsigned int) buffer[j]);
            }
            break;
        default:
            break;
        }
    }

    free(buffer);
}
