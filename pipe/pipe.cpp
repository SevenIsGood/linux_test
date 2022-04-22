#include <unistd.h>

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {

    int fd[2];

    pid_t pid;

    if (pipe(fd) < 0) {
        printf("create pipe error\n");
        return 0;
    }
    else if ((pid = fork()) < 0) {
        printf("fork error\n");
        return 0;
    }
    else if (pid) {
        // parent
        close(fd[0]);
        const char *msg = "hello, child!";
        write(fd[1], msg, strlen(msg) + 1);
        printf("parent write: %s\n", msg);
    }
    else {
        // child
        close(fd[1]);
        char msg[100] = {0};
        read(fd[0], msg, sizeof(msg));
        printf("child recevied: %s\n", msg);
    }
    return 0;
}
