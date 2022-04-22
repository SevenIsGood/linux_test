#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char **argv)
{
    int sockfd;

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
    {
        perror("sock open error:");
        return 0;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0)
    {
        perror("inet_pton error:");
        return 0;
    }

    if (connect(sockfd, (const sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect error:");
        return 0;
    }

    char recvBuf[1024] = {0};

    while (1)
    {
        int readLen = recv(sockfd, recvBuf, sizeof(recvBuf), 0);
        if (readLen <= 0)
        {
            perror("recv error:");
            fprintf(stderr, " readLen %d, errno %d\n", readLen, errno);
            return 0;
        }
        else
        {
            printf("recv: %s\n", recvBuf);
        }
    }

    while (1)
    {
        sleep(1);
    }

    return 0;
}
