////http://stackoverflow.com/questions/16159203/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Use struct instead of C++ Point class
typedef struct {
    int x, y;
} Point;

// Global variables to simulate C++ static member variables
Point *currentPos;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

// Function to update position, analogous to A::f() in C++
void* f(void *arg) {
    while (currentPos != NULL) {
        pthread_mutex_lock(&mtx);
        if (currentPos->x + 1 != currentPos->y) {
            printf("%d------- %d\n", currentPos->x, currentPos->y);
            exit(1);
        }
        pthread_mutex_unlock(&mtx);
    }
}

// Thread function, simulating C++ threadFunction
void* threadFunction(void *arg) {
    while (1) {
        Point* addr = malloc(sizeof(Point));
        addr->x = currentPos->x + 1;
        addr->y = currentPos->y + 1;
        currentPos = addr;
    }
}

int main() {
    pthread_t t1, t2;

    // Initialize currentPos
    currentPos = malloc(sizeof(Point));
    currentPos->x = 1;
    currentPos->y = 2;

    // Create threads
    if (pthread_create(&t1, NULL, &f, NULL)) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }

    if (pthread_create(&t2, NULL, &threadFunction, NULL)) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }

    // Wait for threads to finish, although they will never actually reach here
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    free(currentPos);
    return 0;
}