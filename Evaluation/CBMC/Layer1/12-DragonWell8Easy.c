#include <pthread.h>
#include <assert.h>
#include <stdlib.h>

struct PackageEntryTable {
    int init_flag;
};

struct PackageEntryTable* globalPackageTable = NULL;
struct PackageEntryTable dummy = {0};

void* thread_init(void* arg) {
    if (globalPackageTable == NULL) {
        dummy.init_flag = 1;
        globalPackageTable = &dummy;
    }
    return NULL;
}

void* thread_use(void* arg) {
    struct PackageEntryTable* table = globalPackageTable;
    if (table != NULL) {
        assert(table->init_flag == 1);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_init, NULL);
    pthread_create(&t2, NULL, thread_use, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}