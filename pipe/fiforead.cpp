#include <sys/stat.h>
#include <sys/file.h>

#include <stdio.h>
#include <unistd.h>

const char *fifo = "testfifo";

int main(int argc, char **argv)
{
    if (access(fifo, F_OK) < 0)
    {
        if (mkfifo(fifo, S_IRUSR | S_IWUSR) < 0)
        {
            perror("mk fifo error: ");
            return 0;
        }
    }

    int fd = open(fifo, O_RDONLY);
    if (!fd)
    {
        perror("open fifo: ");
        return 0;
    }

    char buf[128] = {0};

    int readLen = 0;
    while (1)
    {
        readLen = read(fd, buf, sizeof(buf));
        if (readLen < 0)
        {
            perror("read fifo error: ");
            return 0;
        }
        else if (readLen == 0)
        {
            printf("fifo closed\n");
            return 0;
        }
        else
        {
            printf("read fifo: %s\n", buf);
        }
    }

    return 0;
}
