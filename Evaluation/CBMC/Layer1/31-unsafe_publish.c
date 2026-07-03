#include <pthread.h>
#include <assert.h>
#include <stddef.h>

typedef struct {
    int x;
    int y;
} Point;

Point *current_pos = NULL;
Point global_p;

void *writer(void *arg) {
    global_p.x = 1;
    global_p.y = 2;
    // Publication
    current_pos = &global_p;
    return NULL;
}

void *reader(void *arg) {
    Point *p = current_pos;
    if (p != NULL) {
        // If Store-Store reordering happens, p->x or p->y might read uninitialized values
        assert(p->x + 1 == p->y);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, writer, NULL);
    pthread_create(&t2, NULL, reader, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}