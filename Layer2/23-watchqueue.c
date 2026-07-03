//BUG：Configuration race between watch queue setup (ioctl) and key linking (keyctl).
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>

int pipe_fds[2];
int key_id;

void *thread_setup(void *arg) {
    ioctl(pipe_fds[0], _IOW('W', 0x60, int), 0x10);
    return NULL;
}

void *thread_watch(void *arg) {
    syscall(__NR_keyctl, 17, key_id, pipe_fds[0], 0);
    return NULL;
}

int main() {
    key_id = syscall(__NR_add_key, "user", "syz", "data", 4, -2); 
    pipe2(pipe_fds, O_NOTIFICATION_PIPE);     pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_setup, NULL);
    pthread_create(&t2, NULL, thread_watch, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}