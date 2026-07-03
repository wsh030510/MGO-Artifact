#include <pthread.h>
#include <assert.h>
#include <stddef.h>

typedef struct {
    int x, y;
} Point;

Point *currentPos = NULL;
Point p;

void* thread_write(void *arg) {
    p.x = 1;
    p.y = 2;
    currentPos = &p;
    return NULL;
}

void* thread_read(void *arg) {
    Point* local = currentPos;
    if (local != NULL) {
        assert(local->x + 1 == local->y);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_write, NULL);
    pthread_create(&t2, NULL, thread_read, NULL);
    return 0;
}