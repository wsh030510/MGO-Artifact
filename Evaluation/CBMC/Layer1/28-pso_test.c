#include <pthread.h>
#include <assert.h>

int pso_x = 0;
int pso_y = 0;
int pso_r1 = 0;
int pso_r2 = 0;

void *thread_writer(void *arg) {
    pso_x = 1;
    pso_y = 1;
    return NULL;
}

void *thread_reader(void *arg) {
    pso_r1 = pso_y; 
    pso_r2 = pso_x; 
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_writer, NULL);
    pthread_create(&t2, NULL, thread_reader, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
       
    assert(!(pso_r1 == 1 && pso_r2 == 0));
    return 0;
}