#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_THREADS 2

static pthread_mutex_t globMutex = PTHREAD_MUTEX_INITIALIZER;
static volatile int glob = 0;

static unsigned long getUnsignedLong(const char *str);
static void *f(void *arg);

int main(int argc, char *argv[]) {
    int numLoops = (argc > 1) ? getUnsignedLong(argv[1]) : 10000000;

    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        int error = pthread_create(&threads[i], NULL, f, &numLoops);
        if (error != 0) {
            fprintf(stderr, "Failed to create thread - %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        int error = pthread_join(threads[i], NULL);
        if (error != 0) {
            fprintf(stderr, "Failed to join thread - %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }

    printf("glob = %d\n", glob);

    return EXIT_SUCCESS;
}

unsigned long getUnsignedLong(const char *str) {
    char *endptr;
    errno = 0;
    unsigned long result = strtoul(str, &endptr, 0);
    if (errno != 0 || *endptr != '\0') {
        fprintf(stderr, "Failed to convert %s to unsigned long - %s\n", str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}

void *f(void *arg) {
    int numLoops = *((int *) arg);

    for (int i = 0; i < numLoops; ++i) {
        int error = pthread_mutex_lock(&globMutex);
        if (error != 0) {
            fprintf(stderr, "Failed to lock mutex - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        int loc = glob;
        glob = ++loc;

        error = pthread_mutex_unlock(&globMutex);
        if (error != 0) {
            fprintf(stderr, "Failed to unlock mutex - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    return NULL;
}
