//https://github.com/includeos/IncludeOS/pull/1926/files

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

const int VMXNET3_NUM_RX_DESC = 512; // Assumed number of receive descriptors
const int VMXNET3_RX_FILL = 2; // Assumed number of packets to fill
const int MAX_PACKET_LEN = 1500; // Assumed maximum packet length

// Assumed descriptor structure
struct Descriptor {
    int address; // Packet address
    int length; // Packet length
};

struct Descriptor descriptors[512];
int product_count = 0;
int consumed_count = 0;

void* refill(void* arg) {
    while (consumed_count < product_count) {
        descriptors[consumed_count % VMXNET3_NUM_RX_DESC].address = 1111; // Example address
        descriptors[consumed_count % VMXNET3_NUM_RX_DESC].length = MAX_PACKET_LEN;
        consumed_count++;
        printf("Consumed: %d\n", consumed_count);
    }
}

void* receive_handler(void* arg){
    while (product_count < VMXNET3_RX_FILL) {
        int idx = product_count;
        descriptors[idx % VMXNET3_NUM_RX_DESC].address = 1111; // Example address
        descriptors[idx % VMXNET3_NUM_RX_DESC].length = MAX_PACKET_LEN;
        //  __arch_read_memory_barrier();
        product_count++;
        printf("Produced: %d\n", idx + 1);
    }
}

int main() {
    pthread_t producer_thread, consumer_thread;

    pthread_create(&producer_thread, NULL, receive_handler, NULL);
    pthread_create(&consumer_thread, NULL, refill, NULL);

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    return 0;
}