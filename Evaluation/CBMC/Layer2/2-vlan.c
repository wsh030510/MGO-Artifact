#include <pthread.h>

struct ifreq { char ifr_name[16]; struct { int sa_family; char sa_data[14]; } ifr_hwaddr; };
int ioctl(int fd, unsigned long request, ...) { return 0; }

int s_igmp = 4, s_kcm = 5;

void *thread_config(void *arg) {
    struct ifreq ifr = { .ifr_name = "syzkaller1" };
    ioctl(s_igmp, 0x8983, &ifr);
    return NULL;
}

void *thread_transmit(void *arg) {
    struct ifreq ifr = { .ifr_name = "syzkaller1", .ifr_hwaddr = { .sa_family = 1 } };
    ioctl(s_kcm, 0x8914, &ifr);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_config, NULL);
    pthread_create(&t2, NULL, thread_transmit, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}