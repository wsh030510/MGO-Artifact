#include <pthread.h>

struct ifreq { char ifr_name[16]; struct { int sa_family; char sa_data[14]; } ifr_hwaddr; };
int ioctl(int fd, unsigned long request, ...) { return 0; }

int s_kcm = 4, s_igmp = 5;

void *thread_vlan(void *arg) {
    struct ifreq ifr = { .ifr_name = "syzkaller1" };
    ioctl(s_igmp, 0x8983, &ifr);
    return NULL;
}

void *thread_hw_write(void *arg) {
    struct ifreq ifr = { .ifr_name = "syzkaller1", .ifr_hwaddr = { .sa_family = 1 } };
    ioctl(s_kcm, 0x8924, &ifr);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_vlan, NULL);
    pthread_create(&t2, NULL, thread_hw_write, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}