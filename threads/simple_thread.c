#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void *f(void *arg);

int main(int argc, char *argv[]) {
    pthread_t thread;
    int result = pthread_create(&thread, NULL, f, "Hello thread");
    if (result != 0) {
        fprintf(stderr, "Failed to create thread - %s\n", strerror(result));
        exit(EXIT_FAILURE);
    }

    printf("Message from main\n");

    void *length;
    result = pthread_join(thread, &length);
    if (result != 0) {
        fprintf(stderr, "Failed to join with thread - %s\n", strerror(result));
        exit(EXIT_FAILURE);
    }

    printf("Thread returned: %ld\n", (long) length);
    

    return EXIT_SUCCESS;
}

void *f(void *arg) {
    printf("%s\n", (const char *) arg);
    return (void *) strlen(arg);
}
