#include <sys/stat.h>
#include <sys/file.h>

#include <stdio.h>
#include <unistd.h>

const char *fifo = "testfifo";

int main(int argc, char **argv) {
    int fd = open(fifo, O_WRONLY);
    if (!fd) {
        perror("open fifo: ");
        return 0;
    }

    char str[] = "test fifo!";
    int len = sizeof(str);

    if (len != write(fd, str, len)) {
        perror("write fifo error: ");
        return 0;
    }
    else {
        printf("write fifo: %s\n", str);
    }

    sleep(3);

    return 0;
}