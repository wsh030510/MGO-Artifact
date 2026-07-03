//BUG：Load-Load reordering during VMCI wait queue polling (vmci_host_poll).
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <pthread.h>

int epfd, vmci_fd, mice_fd;

void *thread_vmci(void *arg) {
    // ioctl$IOCTL_VMCI_VERSION2
    ioctl(vmci_fd, 0x7a7, 0x10000);
    // ioctl$IOCTL_VMCI_INIT_CONTEXT
    ioctl(vmci_fd, 0x7a0, 0);
    return NULL;
}

void *thread_epoll(void *arg) {
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = vmci_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, vmci_fd, &ev);
    return NULL;
}

int main() {
    epfd = epoll_create1(0);
    vmci_fd = open("/dev/vmci", O_RDWR); 
    mice_fd = open("/dev/input/mice", O_RDONLY);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_vmci, NULL);
    pthread_create(&t2, NULL, thread_epoll, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}