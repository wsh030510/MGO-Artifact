#include <pthread.h>
#include <assert.h>
#include <stdbool.h>

bool active = false;
int status = -1;

void* thread_wait(void* arg) {
    active = true;
    __CPROVER_assume(!active);
    assert(status == 0);
    return NULL;
}

void* thread_callback(void* arg) {
    status = 0;
    active = false;
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_wait, NULL);
    pthread_create(&t2, NULL, thread_callback, NULL);
    return 0;
}