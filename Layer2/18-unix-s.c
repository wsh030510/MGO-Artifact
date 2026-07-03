
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


void *thread_0(void *arg) {
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path + 1, "syz_strict");
    
    bind(u_sock, (struct sockaddr*)&addr, sizeof(sa_family_t) + 11);
    return NULL;
}

// Thread 1: Listen & Ioctl
void *thread_1(void *arg) {
    usleep(1000); 
    listen(u_sock, 0);
    ioctl(u_sock, SIOCX25GSUBSCRIP, 0);
    return NULL;
}

int main() {
    u_sock = socket(AF_UNIX, SOCK_SEQPACKET, 0);

    pthread_t t0, t1;
    pthread_create(&t0, NULL, thread_0, NULL);
    pthread_create(&t1, NULL, thread_1, NULL);

    pthread_join(t0, NULL);
    pthread_join(t1, NULL);
    
    close(u_sock);
    return 0;
}