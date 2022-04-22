#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <errno.h>

int main(int argc, char **argv) {
    struct sockaddr_in addr;
    memset((void *)&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int listener = socket(AF_INET, SOCK_STREAM, 0);

    if (listener < 0) {
        perror("socket:");
        return 0;
    }

    int use = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &use, (socklen_t)sizeof(use)) < 0) {
        perror("setsockopt:");
        return 0;
    }

    socklen_t addrLen = sizeof(addr);

    if (bind(listener, (struct sockaddr *)&addr, addrLen) < 0) {
        perror("bind:");
        return 0;
    }

    if (listen(listener, 16) < 0) {
        perror("listen:");
        return 0;
    }

    struct sockaddr_in clientAddr;
    int client = -1;

    if ((client = accept(listener, (struct sockaddr *)&clientAddr, &addrLen)) < 0) {
        perror("accept:");
        return 0;
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
#else
    const char *testStr = "test";
    strcpy(sendBuf, testStr);
    len = strlen(testStr) + 1;
    int sendCount = 0;

    int sendBufLen = 0;
    int argLen = sizeof(sendBufLen);

    if (getsockopt(client, SOL_SOCKET, SO_SNDBUF, &sendBufLen, (socklen_t *)&argLen) < 0) {
        perror("getsockopt SO_SNDBUF error:");
        return 0;
    }
    else {
        printf("send buf len: %d\n", sendBufLen);
    }

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    if (setsockopt(client, SOL_SOCKET, SO_SNDTIMEO, &timeout, (socklen_t)sizeof(timeout)) < 0) {
        perror("setsockopt SO_SNDTIMEO error:");
        return 0;
    }

    int reuse = 1;
    if (setsockopt(client, SOL_SOCKET, SO_REUSEADDR, &reuse, (socklen_t)argLen) < 0) {
        perror("setsockopt SO_REUSEADDR error:");
        return 0;
    }

    while(1) {
        int ret = send(client, sendBuf, len, MSG_NOSIGNAL);

        if (ret <= 0) {
            perror("send error:");

            fprintf(stderr, "send count: %d, ret %d, errno %d\n", sendCount, ret, errno);

            return 0;
        }

        sendCount += ret;
    }

#endif

    return 0;
}
