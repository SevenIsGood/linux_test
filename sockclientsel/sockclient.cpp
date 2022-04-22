#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <signal.h>

int SockVec[20];

typedef void Sigfunc(int);
Sigfunc *signal(int, Sigfunc *);

void S(int sig)
{
    if (sig == SIGINT)
    {
        fprintf(stderr, "kill signal received, close all socket...\n");
        for (int i = 0; i < sizeof(SockVec) / sizeof(int); ++i)
        {
            //shutdown(SockVec[i], SHUT_RDWR);
            close(SockVec[i]);
        }
    }

    exit(0);
}

int main(int argc, char **argv)
{
    if (signal(SIGINT, S) == SIG_ERR) {
        fprintf(stderr, "set signal action error\n");
        return 0;
    }

    int N = 0;
    int sockfd;
    while (1)
    {

        sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sockfd < 0)
        {
            perror("sock open error:");
            return 0;
        }

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(13107);
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

        // sleep(10);

        // printf("close socket\n");
        // close(sockfd);

        SockVec[N] = sockfd;

        printf("Test seq %d\n", N);

        ++N;

        if (N >= 20)
        {
            sleep(300000);
        }
        else
        {
            sleep(1);
        }
    }

    char recvBuf[1024] = {0};

    while (1)
    {
        fd_set rset;
        fd_set wset;
        FD_ZERO(&rset);
        FD_ZERO(&wset);

        FD_SET(sockfd, &rset);
        FD_SET(sockfd, &wset);

        timeval to;
        to.tv_sec = 1;
        to.tv_usec = 0;

        int ret = select(sockfd + 1, &rset, &wset, NULL, &to);

        if (ret < 0)
        {
            perror("select error:");
            return 0;
        }
        else if (ret == 0)
        {
            fprintf(stdout, "select timeout\n");
        }
        else
        {
            if (FD_ISSET(sockfd, &rset))
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

                    // shutdown(sockfd, SHUT_WR);

                    sleep(10);

                    printf("shut down send\n");
                }
            }
        }
    }

    while (1)
    {
        sleep(1);
    }

    return 0;
}
