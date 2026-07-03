/*
 * Bug Type: Unsafe Publication / Store-Store Reordering
 * Origin: https://stackoverflow.com/questions/16159203/why-does-this-java-program-terminate-despite-that-apparently-it-shouldnt-and-d
 */

#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    int x;
    int y;
} Point;

// Shared global pointer (Publication point)
// volatile is used here to prevent COMPILER from optimizing away the reads,
// but it does NOT prevent CPU reordering on ARM.
Point * volatile current_pos = NULL;

// Pre-allocated arena to avoid malloc() barriers masking the bug
#define ARENA_SIZE 1000000
Point arena[ARENA_SIZE];

// Thread 1: Writer (Simulates main thread in Java)
void *writer(void *arg) {
    int seed = 1;
    int idx = 0;
    
    while (1) {
        // 1. "new Point" - Grab a slot from our arena
        Point *p = &arena[idx];
        
        // 2. Constructor Logic - Initialize fields
        // In the bug scenario, these stores happen...
        p->x = seed;
        p->y = seed + 1;

        // --- BUG: MISSING WRITE BARRIER (smp_wmb) HERE ---
        // On ARM, the CPU pipeline may execute step 3 before step 2 completes visibility.
        
        // 3. Publication - Publish the reference
        current_pos = p;

        // Move to next slot and seed
        idx = (idx + 1) % ARENA_SIZE;
        seed++;
    }
    return NULL;
}

// Thread 2: Reader (Simulates the anonymous Thread in Java)
void *reader(void *arg) {
    // Wait until at least one object is published
    while (current_pos == NULL);

    while (1) {
        // Read the shared pointer
        Point *p = current_pos;

        if (p != NULL) {
            // Read fields
            // On ARM, without a data dependency barrier (or smp_rmb),
            // we might see the new pointer 'p' but old values in 'p->x' or 'p->y'
            int x = p->x;
            int y = p->y;

            // Check invariant
            if (x + 1 != y) {
                printf("[!] OOO Bug Detected!\n");
                printf("    Pointer: %p\n", p);
                printf("    Read: x=%d, y=%d (Expected y to be %d)\n", x, y, x + 1);
                exit(1);
            }
        }
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    printf("[*] Starting Unsafe Publication Test...\n");

    // Create threads
    pthread_create(&t2, NULL, reader, NULL); // Reader
    pthread_create(&t1, NULL, writer, NULL); // Writer

    pthread_join(t2, NULL); 
    // Writer runs infinitely, so we only join reader if it crashes
    
    return 0;
}