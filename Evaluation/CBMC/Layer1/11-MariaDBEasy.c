#include <pthread.h>
#include <assert.h>
#include <stddef.h>

int node_key = 1;
int node_link = 1;

void* thread_delete(void* arg) {
    node_link = 0; 
    node_key = 0;  
    return NULL;
}

void* thread_find(void* arg) {
    int k = node_key;
    int l = node_link;
    if (l == 1) {
        assert(k == 1);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_delete, NULL);
    pthread_create(&t2, NULL, thread_find, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}