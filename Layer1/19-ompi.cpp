// https://github.com/open-mpi/ompi/pull/5536
#include <pthread.h>
#include <unistd.h>
#include <atomic>
#include <iostream>
// Define header structure
class mca_btl_vader_fbox_hdr_t {
public:
    uint32_t size;
    uint16_t tag;
    uint16_t seq;
    mca_btl_vader_fbox_hdr_t(uint16_t t, uint16_t s, uint32_t sz) : tag(t), seq(s), size(sz) {}
    mca_btl_vader_fbox_hdr_t() : tag(0), seq(0), size(0) {}
};

// Write memory barrier
static inline void opal_atomic_wmb(void) {
    std::atomic_thread_fence(std::memory_order_release);
}

// Read memory barrier
static inline void opal_atomic_rmb(void) {
    std::atomic_thread_fence(std::memory_order_acquire);
}

// Global header structure
mca_btl_vader_fbox_hdr_t* global_hdr;

// Set header structure values and insert write memory barrier
static inline void mca_btl_vader_fbox_set_header(uint16_t tag,
                                                 uint16_t seq, uint32_t size)
{
    mca_btl_vader_fbox_hdr_t tmp(tag, seq, size);
    global_hdr = &tmp;
    opal_atomic_wmb();
}

// Read header structure values and insert read memory barrier
static inline void mca_btl_vader_fbox_read_header()
{
    std::cout<<global_hdr;
}

// Writer thread function
void* writer_thread(void* arg) {
    for (int i = 0; i < 5; ++i) {
        mca_btl_vader_fbox_set_header(1, i, 1024);
        sleep(1); // Simulate interval between write operations
    }
    return NULL;
}

// Reader thread function
void* reader_thread(void* arg) {
    for (int i = 0; i < 5; ++i) {
        mca_btl_vader_fbox_read_header();
        sleep(1); // Simulate interval between read operations
    }
    return NULL;
}

// Main function
int main() {
    pthread_t writer, reader;
    pthread_create(&writer, NULL, writer_thread, NULL);
    pthread_create(&reader, NULL, reader_thread, NULL);
    pthread_join(writer, NULL);
    pthread_join(reader, NULL);
    return 0;
}