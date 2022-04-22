#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <errno.h>

int main(int argc, char **argv)
{
    struct sockaddr_in addr;
    memset((void *)&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int listener = socket(AF_INET, SOCK_STREAM, 0);

    if (listener < 0)
    {
        perror("socket:");
        return 0;
    }

    int use = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &use, (socklen_t)sizeof(use)) < 0)
    {
        perror("setsockopt:");
        return 0;
    }

    socklen_t addrLen = sizeof(addr);

    if (bind(listener, (struct sockaddr *)&addr, addrLen) < 0)
    {
        perror("bind:");
        return 0;
    }

    if (listen(listener, 16) < 0)
    {
        perror("listen:");
        return 0;
    }

    while (1)
    {
        struct sockaddr_in clientAddr;
        int client = -1;

        if ((client = accept(listener, (struct sockaddr *)&clientAddr, &addrLen)) < 0)
        {
            perror("accept:");
            return 0;
        }
        else
        {
            char buf[50] = {0};

            inet_ntop(AF_INET, &clientAddr.sin_addr, buf, sizeof(buf));
            printf("---------accept: %s:%d\n", buf, ntohs(clientAddr.sin_port));
        }

        char buf[1024];
        int len = 0;
        char sendBuf[1024];

#if 0
    while((len = recv(client, buf, sizeof(buf), 0)) > 0) {
        buf[len] = '\0';

        printf("received: %s\n", buf);
        
        const char *ret = "Get:";
        strcpy(sendBuf, ret);
        strcat(sendBuf, buf);
        int sendLen = strlen(sendBuf);

       if (sendLen != send(client, sendBuf, sendLen, 0)) {
            perror("send:");
            return 0;
        }
    }
#endif

        const char *testStr = "test";
        strcpy(sendBuf, testStr);
        len = strlen(testStr) + 1;
        int sendCount = 0;

        int sendBufLen = 0;
        int argLen = sizeof(sendBufLen);

        if (getsockopt(client, SOL_SOCKET, SO_SNDBUF, &sendBufLen, (socklen_t *)&argLen) < 0)
        {
            perror("getsockopt SO_SNDBUF error:");
            return 0;
        }
        else
        {
            printf("send buf len: %d\n", sendBufLen);
        }

        struct timeval timeout;
        timeout.tv_sec = 30;
        timeout.tv_usec = 0;

        if (setsockopt(client, SOL_SOCKET, SO_SNDTIMEO, &timeout, (socklen_t)sizeof(timeout)) < 0)
        {
            perror("setsockopt SO_SNDTIMEO error:");
            return 0;
        }

        int reuse = 1;
        if (setsockopt(client, SOL_SOCKET, SO_REUSEADDR, &reuse, (socklen_t)argLen) < 0)
        {
            perror("setsockopt SO_REUSEADDR error:");
            return 0;
        }

        char recvBuf[1024] = {0};

        while (1)
        {
            {
                int ret = send(client, sendBuf, len, MSG_NOSIGNAL);
                if (ret <= 0)
                {
                    perror("send error:");

                    fprintf(stderr, "send count: %d, ret %d, errno %d\n", sendCount, ret, errno);

                    sleep(3);
                    close(client);
                    fprintf(stderr, "close socket\n");
                    break;
                }
                else
                {
                    printf("send %d bytes\n", ret);
                    continue;
                }
            }

            fd_set rset;
            fd_set wset;
            FD_ZERO(&rset);
            FD_ZERO(&wset);

            FD_SET(client, &rset);
            FD_SET(client, &wset);

            timeval to;
            to.tv_sec = 1;
            to.tv_usec = 0;

            int ret = select(client + 1, &rset, &wset, NULL, &to);

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
#if 0
                if (FD_ISSET(client, &rset))
                {
                    memset(recvBuf, 0, sizeof(recvBuf));
                    int readLen = recv(client, recvBuf, sizeof(recvBuf), 0);
                    if (readLen < 0)
                    {
                        perror("recv error:");
                        fprintf(stderr, " readLen %d, errno %d.\n", readLen, errno);

                        close(client);
                        fprintf(stderr, "close socket\n");
                        break;
                    }
                    else if (readLen == 0)
                    {
                        fprintf(stderr, "read 0 bytes, peer closed. readLen %d, errno %d. \n", readLen, errno);

                        close(client);
                        fprintf(stderr, "close socket\n");
                        break;
                    }
                    else
                    {
                        printf("recv: %s\n", recvBuf);
                    }
                }
#endif

                if (FD_ISSET(client, &wset))
                {
                    int ret = send(client, sendBuf, len, MSG_NOSIGNAL);
                    if (ret <= 0)
                    {
                        perror("send error:");

                        fprintf(stderr, "send count: %d, ret %d, errno %d\n", sendCount, ret, errno);

                        close(client);
                        fprintf(stderr, "close socket\n");
                        break;
                    }
                    else
                    {
                        printf("send %d bytes\n", ret);
                    }

                    sendCount += ret;
                }
            }

            // sleep(1);
        }
    }

    return 0;
}
