#include <pthread.h>
#include <assert.h>

int tso_x = 0;
int tso_y = 0;
int tso_r1 = 0;
int tso_r2 = 0;

void *thread_1(void *arg) {
    tso_x = 1;
    tso_r1 = tso_y;
    return NULL;
}

void *thread_2(void *arg) {
    tso_y = 1;
    tso_r2 = tso_x;
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_1, NULL);
    pthread_create(&t2, NULL, thread_2, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
        
    assert(!(tso_r1 == 0 && tso_r2 == 0));
    return 0;
}