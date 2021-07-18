#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int glob = 0;

static void memoryCleanupHandler(void *arg);
static void mutexCleanupHandler(void *arg);
static void *threadFunc(void *arg);

int main(int argc, char *argv[]) {
    pthread_t thread;
    int result = pthread_create(&thread, NULL, threadFunc, NULL);
    if (result != 0) {
        fprintf(stderr, "pthread_create() failed - %s\n", strerror(result));
        exit(EXIT_FAILURE);
    }

    // Allow thread to execute
    sleep(2);

    if (argc == 1) {
        // Cancel thread
        printf("main:    about to cancel thread\n");
        result = pthread_cancel(thread);
        if (result != 0) {
            fprintf(stderr, "pthread_cancel() failed - %s\n", strerror(result));
            exit(EXIT_FAILURE);
        }
    } else {
        // Change global variable and notify thread
        printf("main:    about to signal condition variable\n");
        result = pthread_mutex_lock(&mutex);
        if (result != 0) {
            fprintf(stderr, "pthread_mutex_lock() failed - %s\n", strerror(result));
            exit(EXIT_FAILURE);
        }

        glob = 1;

        result = pthread_mutex_unlock(&mutex);
        if (result != 0) {
            fprintf(stderr, "pthread_mutex_unlock() failed - %s\n", strerror(result));
            exit(EXIT_FAILURE);
        }

        result = pthread_cond_signal(&cond);
        if (result != 0) {
            fprintf(stderr, "pthread_cond_signal() failed - %s\n", strerror(result));
            exit(EXIT_FAILURE);
        }
    }

    void *status;
    result = pthread_join(thread, &status);
    if (result != 0) {
        fprintf(stderr, "pthread_join() failed - %s\n", strerror(result));
        exit(EXIT_FAILURE);
    }

    if (status == PTHREAD_CANCELED) {
        printf("main:    thread was cancelled\n");
    } else {
        printf("main:    thread terminated normally\n");
    }

    return EXIT_SUCCESS;
}

void memoryCleanupHandler(void *arg) {
    printf("cleanup: freeing block at %p\n", arg);
    free(arg);
}

void mutexCleanupHandler(void *arg) {
    printf("cleanup: unlocking mutex\n");
    int result = pthread_mutex_unlock(&mutex);
    if (result != 0) {
        fprintf(stderr, "pthread_mutex_unlock() failed - %s\n", strerror(result));
        exit(EXIT_FAILURE);
    }
}

void *threadFunc(void *arg) {
    // Allocate buffer
    void *buffer = malloc(0x10000);
    if (buffer == NULL) {
        fprintf(stderr, "malloc() failed - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    pthread_cleanup_push(memoryCleanupHandler, buffer);
    printf("thread:  allocated memory at %p\n", buffer);

    // Wait for glob change
    int result = pthread_mutex_lock(&mutex);
    if (result != 0) {
        fprintf(stderr, "pthread_mutex_lock() failed - %s\n", strerror(result));
        exit(EXIT_FAILURE);
    }

    pthread_cleanup_push(mutexCleanupHandler, NULL);
    
    while (glob == 0) {
        result = pthread_cond_wait(&cond, &mutex);
        if (result != 0) {
            fprintf(stderr, "pthread_cond_wait() failed - %s\n", strerror(result));
            exit(EXIT_FAILURE);
        }
    }

    printf("thread: condition wait loop completed\n");
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);

    return NULL;
}
