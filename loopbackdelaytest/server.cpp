#include <stdio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);

    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) != 1) {
        perror("inet_pton error:");
        return 0;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("create sock error:");
        return 0;
    }

    int reuse = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt reuse error:");
        return 0;
    }

    if (bind(sock, (const sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) {
        perror("bind error:");
        return 0;
    }

    if (listen(sock, 10) < 0) {
        perror("listen error:");
        return 0;
    }

    struct sockaddr_in client;
    int addrLen = sizeof(sockaddr_in);

    int clientSock;
    if ((clientSock = accept(sock, (sockaddr *)&client, (socklen_t *)&addrLen)) < 0) {
        perror("accept error:");
        return 0;
    }

    char buf[INET_ADDRSTRLEN];
    if (!inet_ntop(AF_INET, &client.sin_addr, buf, INET_ADDRSTRLEN)) {
        perror("client addr to str error:");
        return 0;
    }

    printf("accept client %s, port %d\n", buf, ntohs(client.sin_port));

    char sendBuf[] = "send test message!";
    int sendLen = 0;
    sendLen = send(clientSock, sendBuf, sizeof(sendBuf), 0);
    if (sendLen < 0) {
        perror("server send error:");
        return 0;
    }

    struct timeval tv;
    gettimeofday(&tv, 0);
    printf("server send message at: %ld.%06d, message: %s\n",  tv.tv_sec, tv.tv_usec, sendBuf);

    return 0;
}

