#include <pthread.h>

int ioctl(int fd, unsigned long request, ...) { return 0; }
int syscall(int number, ...) { return 0; }
int pipe2(int pipefd[2], int flags) { pipefd[0]=3; pipefd[1]=4; return 0; }

int pipe_fds[2];
int key_id;

void *thread_setup(void *arg) {
    ioctl(pipe_fds[0], 0x40045760, 0x10);
    return NULL;
}

void *thread_watch(void *arg) {
    syscall(250, 17, key_id, pipe_fds[0], 0);
    return NULL;
}

int main() {
    key_id = syscall(248, "user", "syz", "data", 4, -2); 
    pipe2(pipe_fds, 0); 
    
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_setup, NULL);
    pthread_create(&t2, NULL, thread_watch, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}