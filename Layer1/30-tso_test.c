/* tso_test.c */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Shared variables
int x = 0, y = 0;
// Observation results
int r1 = 0, r2 = 0;

sem_t start_sem;

void *thread_1(void *arg) {
    sem_wait(&start_sem);
    // TSO Pattern: Store X -> Load Y
    x = 1;
    // Missing Memory Barrier Here (MFENCE)
    // asm volatile("mfence" ::: "memory"); 
    r1 = y;
    return NULL;
}

void *thread_2(void *arg) {
    sem_wait(&start_sem);
    // TSO Pattern: Store Y -> Load X
    y = 1;
    // Missing Memory Barrier Here
    r2 = x;
    return NULL;
}

int main() {
    pthread_t t1, t2;
    sem_init(&start_sem, 0, 0);
    for(int i=0; i<1000; i++) {
        x = 0; y = 0;
        pthread_create(&t1, NULL, thread_1, NULL);
        pthread_create(&t2, NULL, thread_2, NULL);
        sem_post(&start_sem);
        sem_post(&start_sem);
        
        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
        
        if (r1 == 0 && r2 == 0) {
            printf("[!] TSO Violation Detected at iteration %d: r1=0, r2=0\n", i);
            break;
        }
    }
    printf("TSO Test Finished.\n");
    return 0;
}