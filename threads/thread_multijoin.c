#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum ThreadState {
    TS_ALIVE,
    TS_TERMINATED,
    TS_JOINED
};

struct Thread {
    pthread_t id;
    enum ThreadState state;
    int sleepTime_sec;
};

static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static struct Thread *threads;
static int numUnjoinedThreads = 0;

static int getInt(const char *str);
static void *func(void *arg);

int main(int argc, char *argv[]) {
    if (argc < 2 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s <nsecs>...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const int numThreads = argc - 1;
    threads = malloc(numThreads * sizeof(struct Thread));
    if (threads == NULL) {
        fprintf(stderr, "Failed to malloc() memory for threads - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Create threads to sleep for each argument
    for (int i = 0; i < numThreads; ++i) {
        threads[i].state = TS_ALIVE;
        threads[i].sleepTime_sec = getInt(argv[i + 1]);

        int *index = malloc(sizeof(int));
        *index = i;
        if (index == NULL) {
            fprintf(stderr, "Failed to malloc() memory for index - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        int error = pthread_create(&threads[i].id, NULL, func, index);
        if (error != 0) {
            fprintf(stderr, "Failed to create pthread - %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }

    // Join unterminated threads
    for(int numLiveThreads = numThreads; numLiveThreads > 0; ) {
        int error = pthread_mutex_lock(&mutex);
        if (error != 0) {
            fprintf(stderr, "Failed to lock mutex - %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }

        while (numUnjoinedThreads == 0) {
            error = pthread_cond_wait(&cond, &mutex);
            if (error != 0) {
                fprintf(stderr, "Failed to pthread_cond_wait() - %s\n", strerror(error));
                exit(EXIT_FAILURE);
            }
        }
        
        for (int i = 0; i < numThreads; ++i) {
            if (threads[i].state == TS_TERMINATED) {
                error = pthread_join(threads[i].id, NULL);
                if (error != 0) {
                    fprintf(stderr, "Failed to join() with thread %d - %s\n", i, strerror(error));
                    exit(EXIT_FAILURE);
                }

                threads[i].state = TS_JOINED;
                --numLiveThreads;
                --numUnjoinedThreads;

                printf("Reaped thread %d (numLiveThreads = %d)\n", i, numLiveThreads);
            }
        }


        error = pthread_mutex_unlock(&mutex);
        if (error != 0) {
            fprintf(stderr, "Failed to unlock mutex - %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }

    free(threads);

    return EXIT_SUCCESS;
}

int getInt(const char *str) {
    char *endptr;
    errno = 0;
    int result = strtol(str, &endptr, 0);
    if (errno != 0 || *endptr != '\0') {
        fprintf(stderr, "Failed to convert %s to int - %s\n", str, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}

void *func(void *arg) {
    int *index = arg;
    
    sleep(threads[*index].sleepTime_sec);
    printf("Thread %d terminating\n", *index);

    int error = pthread_mutex_lock(&mutex);
    if (error != 0) {
        fprintf(stderr, "Failed to lock mutex - %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
    
    threads[*index].state = TS_TERMINATED;
    ++numUnjoinedThreads;

    error = pthread_mutex_unlock(&mutex);
    if (error != 0) {
        fprintf(stderr, "Failed to unlock mutex - %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }

    error = pthread_cond_signal(&cond);
    if (error != 0) {
        fprintf(stderr, "Failed to pthread_cond_signal() - %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }

    free(index);

    return NULL;
}
