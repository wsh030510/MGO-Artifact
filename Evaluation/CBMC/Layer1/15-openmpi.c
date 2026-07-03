#include <pthread.h>
#include <assert.h>

int list_data = 0;
int head_ptr = 0;

void* thread_push(void* arg) {
    list_data = 42;
    head_ptr = 1;
    return NULL;
}

void* thread_pop(void* arg) {
    if (head_ptr == 1) {
        assert(list_data == 42);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_push, NULL);
    pthread_create(&t2, NULL, thread_pop, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}