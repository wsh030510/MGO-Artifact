/* pso_test.c */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int x = 0, y = 0;
int r1 = 0, r2 = 0;
sem_t start_sem;

void *thread_writer(void *arg) {
    sem_wait(&start_sem);
    // PSO Pattern: Store X -> Store Y
    x = 1;
    // Missing Store Barrier (SFENCE)
    // asm volatile("sfence" ::: "memory");
    y = 1;
    return NULL;
}

void *thread_reader(void *arg) {
    sem_wait(&start_sem);
    // Reader: Check order
    r1 = y; // Read Y first
    // Missing Load Barrier (LFENCE)
    r2 = x; // Then read X
    return NULL;
}

int main() {
    pthread_t t1, t2;
    sem_init(&start_sem, 0, 0);

    for(int i=0; i<1000; i++) {
        x = 0; y = 0;
        pthread_create(&t1, NULL, thread_writer, NULL);
        pthread_create(&t2, NULL, thread_reader, NULL);
        
        sem_post(&start_sem);
        sem_post(&start_sem);
        
        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
       
        if (r1 == 1 && r2 == 0) {
            printf("[!] PSO Violation Detected at iteration %d: Y=1, X=0\n", i);
            break;
        }
    }
    printf("PSO Test Finished.\n");
    return 0;
}