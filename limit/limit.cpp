#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    int fd = open("fd.txt", O_WRONLY | O_CREAT);
    if (fd <= 0) {
        perror("open file error:");
        return 0;
    }

    for (int i = 0; ; ++i) {
        int newFd = dup(fd);
        if (newFd < 0) {
            perror("dup error:");
            fprintf(stderr, "dup fd error at: %d\n", i);
            return 0;
        }
    }
    
    return 0;
}