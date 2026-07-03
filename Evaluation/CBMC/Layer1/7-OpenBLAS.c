#include <pthread.h>
#include <assert.h>

int memory_buffer[1];
int memory_flag = 0;

void* thread_alloc(void* arg) {
    memory_buffer[0] = 42;
    __sync_synchronize(); 
    memory_flag = 1;
    return NULL;
}

void* thread_use(void* arg) {
    if (memory_flag == 1) {
        assert(memory_buffer[0] == 42);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_alloc, NULL);
    pthread_create(&t2, NULL, thread_use, NULL);
    return 0;
}