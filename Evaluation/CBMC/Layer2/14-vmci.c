#include <pthread.h>

int ioctl(int fd, unsigned long request, ...) { return 0; }
int epoll_ctl(int epfd, int op, int fd, void *event) { return 0; }

int epfd = 3, vmci_fd = 4;

void *thread_vmci(void *arg) {
    ioctl(vmci_fd, 0x7a7, 0x10000);
    ioctl(vmci_fd, 0x7a0, 0);
    return NULL;
}

void *thread_epoll(void *arg) {
    struct { unsigned int events; int fd; } ev = {1, vmci_fd}; // EPOLLIN
    epoll_ctl(epfd, 1, vmci_fd, &ev); // EPOLL_CTL_ADD
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_vmci, NULL);
    pthread_create(&t2, NULL, thread_epoll, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}