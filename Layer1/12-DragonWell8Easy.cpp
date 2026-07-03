//https://bugs.openjdk.org/browse/JDK-8154750
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>

#define PACKAGE_TABLE_ENTRY_SIZE 10

typedef struct PackageEntry {
    char* name;
    struct PackageEntry* nextEntry;
} PackageEntry;

struct PackageEntryTable {
    PackageEntry* buckets[PACKAGE_TABLE_ENTRY_SIZE]{};
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

    PackageEntryTable() {
        for (int i = 0; i < PACKAGE_TABLE_ENTRY_SIZE; ++i) {
            buckets[i] = nullptr;
        }
    }
};

// Global variables representing the package table and its mutex; simplified example does not include mutex initialization code
struct PackageEntryTable* globalPackageTable = nullptr;
pthread_mutex_t globalTableLock = PTHREAD_MUTEX_INITIALIZER;

// Ensure PackageEntryTable is created exactly once
PackageEntryTable* getOrCreatePackageEntryTable() {
    // Lazily create the package entry table at first request.
    // Lock-free access requires load_ptr_acquire.
    // PackageEntryTable* packages = load_ptr_acquire(&_packages);
    if (globalPackageTable == nullptr) {
        pthread_mutex_lock(&globalTableLock);
        if (globalPackageTable == nullptr) {
            globalPackageTable = new PackageEntryTable();
            // Ensure _packages is stable, since it is examined without a lock
            // OrderAccess::release_store_ptr(&_packages, packages);
        }
        pthread_mutex_unlock(&globalTableLock);
    }
    return globalPackageTable;
}

void* threadRoutine(void* arg) {
    // Call getOrCreatePackageEntryTable; each thread attempts to get or create the global PackageEntryTable instance
    PackageEntryTable* table = getOrCreatePackageEntryTable();
    printf("Thread %ld has accessed the PackageEntryTable at address %p\n", (long)pthread_self(), (void*)table);

    return NULL;
}

// Example usage
int main() {
    const int NUM_THREADS = 2;
    pthread_t threads[NUM_THREADS];

    // Create multiple threads, each trying to get or create the PackageEntryTable instance
    for (unsigned long & thread : threads) {
        if (pthread_create(&thread, NULL, threadRoutine, NULL) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // Wait for all threads to complete
    for (unsigned long thread : threads) {
        pthread_join(thread, NULL);
    }
}