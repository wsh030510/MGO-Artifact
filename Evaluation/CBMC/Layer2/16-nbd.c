#include <pthread.h>

int sendmsg(int sockfd, const void *msg, int flags) { return 0; }
int ioctl(int fd, unsigned long request, ...) { return 0; }

int nl_sock = 3;
int nbd_fd = 4;

void *thread_netlink(void *arg) {
    int dummy_msg[20] = {0};
    // Reduced iterations to prevent CBMC state explosion
    for(int i = 0; i < 2; i++) {
        sendmsg(nl_sock, dummy_msg, 0);
    }
    return NULL;
}

void *thread_ioctl(void *arg) {
    ioctl(nbd_fd, 0xab00, 0); // NBD_SET_SOCK
    ioctl(nbd_fd, 0xab04, 0); // NBD_DO_IT
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_netlink, NULL);
    pthread_create(&t2, NULL, thread_ioctl, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}