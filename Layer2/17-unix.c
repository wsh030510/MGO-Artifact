//# commit ae3b564179bfd06f32d051b9e5d72ce4b2a07c37
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <string.h>

int u_sock;
#define SIOCX25GSUBSCRIP 0x89e0

void *thread_setup(void *arg) {
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path + 1, "syz_test_sock");
    
    bind(u_sock, (struct sockaddr*)&addr, sizeof(sa_family_t) + 14);
    listen(u_sock, 0);
    return NULL;
}

void *thread_weird_ioctl(void *arg) {
    ioctl(u_sock, SIOCX25GSUBSCRIP, 0);
    return NULL;
}

int main() {
    u_sock = socket(AF_UNIX, SOCK_SEQPACKET, 0); // 0x5 = SOCK_SEQPACKET

    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_setup, NULL);
    pthread_create(&t2, NULL, thread_weird_ioctl, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    close(u_sock);
    return 0;
}