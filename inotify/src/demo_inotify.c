#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <unistd.h>

#define BUFFER_LENGTH (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

static void printInotifyEvent(const struct inotify_event *event);

int main(int argc, char *argv[]) {
    if (argc < 2 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <file>...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int inotifyFd = inotify_init();
    if (inotifyFd == -1) {
        fprintf(stderr, "Failed to initialize inotify instance - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; ++i) {
        const char *filepath = argv[i];
        int wd = inotify_add_watch(inotifyFd, filepath, IN_ALL_EVENTS);
        if (wd == -1) {
            fprintf(stderr, "Failed to add inotify watch to file %s - %s\n", filepath, strerror(errno));
            exit(EXIT_FAILURE);
        }

        printf("Watching %s using wd %d\n", filepath, wd);
    }

    char buffer[BUFFER_LENGTH];
    for (;;) {
        ssize_t numBytesRead = read(inotifyFd, buffer, sizeof(buffer));
        if (numBytesRead <= 0) {
            fprintf(stderr, "Failed to read() inotify fd %d - %s\n", inotifyFd, strerror(errno));
            exit(EXIT_FAILURE);
        }
        
        printf("Read %ld bytes from inotify fd\n", (long) numBytesRead);

        for (char *p = buffer; p < buffer + numBytesRead; p += sizeof(struct inotify_event) + ((struct inotify_event *) p)->len) {
            printInotifyEvent((struct inotify_event *)p);
        }
    }

    return EXIT_SUCCESS;
}

void printInotifyEvent(const struct inotify_event *event) {
    printf("    wd = %d; ", event->wd);
    if (event->len > 0) printf("name = %s; ", event->name);
    if (event->cookie > 0) printf("cookie = %d; ", event->cookie);

    if (event->mask & IN_ACCESS) printf("IN_ACCESS ");
    if (event->mask & IN_ATTRIB) printf("IN_ATTRIB ");
    if (event->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (event->mask & IN_CLOSE_WRITE) printf("IN_CLOSE_WRITE ");
    if (event->mask & IN_CREATE) printf("IN_CREATE ");
    if (event->mask & IN_DELETE) printf("IN_DELETE ");
    if (event->mask & IN_DELETE_SELF) printf("IN_DELETE_SELF ");
    if (event->mask & IN_IGNORED) printf("IN_IGNORED ");
    if (event->mask & IN_ISDIR) printf("IN_ISDIR ");
    if (event->mask & IN_MODIFY) printf("IN_MODIFY ");
    if (event->mask & IN_MOVE_SELF) printf("IN_MOVE_SELF ");
    if (event->mask & IN_MOVED_FROM) printf("IN_MOVED_FROM ");
    if (event->mask & IN_MOVED_TO) printf("IN_MOVED_TO ");
    if (event->mask & IN_OPEN) printf("IN_OPEN ");
    if (event->mask & IN_Q_OVERFLOW) printf("IN_Q_OVERFLOW ");
    if (event->mask & IN_UNMOUNT) printf("IN_UNMOUNT ");
    printf("\n");
}
