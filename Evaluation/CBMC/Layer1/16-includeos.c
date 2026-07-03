#include <pthread.h>
#include <assert.h>

int desc_address = 0;
int product_count = 0;

void* thread_produce(void* arg) {
    desc_address = 1111;
    __sync_synchronize(); /
    product_count = 1;
    return NULL;
}

void* thread_consume(void* arg) {
    if (product_count == 1) {
        assert(desc_address == 1111);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_produce, NULL);
    pthread_create(&t2, NULL, thread_consume, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}