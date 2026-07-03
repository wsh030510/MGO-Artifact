//BUG：Configuration race between GSM line discipline (TIOCSETD) and terminal attributes (TCSETSF).
/* 11-gsm.c */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <pthread.h>
#include <string.h>

// GSM 0710 Mux Line Discipline
#ifndef N_GSM0710
#define N_GSM0710 21
#endif

int fd;

void *thread_ldisc(void *arg) {
    int ldisc = N_GSM0710;
    ioctl(fd, TIOCSETD, &ldisc);
    return NULL;
}

void *thread_term(void *arg) {
    struct termios t;
    memset(&t, 0, sizeof(t));
    t.c_cflag = B9600 | CS8 | CREAD;
    ioctl(fd, TCSETSF, &t);
    return NULL;
}

int main() {
    fd = open("/dev/ptmx", O_RDWR);
    
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_ldisc, NULL);
    pthread_create(&t2, NULL, thread_term, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    close(fd);
    return 0;
}