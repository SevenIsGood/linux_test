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

    if (connect(sock, (const sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) {
        perror("connect to server error:");
        return 0;
    }

    char recvBuf[128] = {0};
    int readLen = 0;
    if ((readLen = recv(sock, recvBuf, sizeof(recvBuf), 0)) < 0) {
        perror("recv error:");
        return 0;
    }
    else if (readLen == 0) {
        printf("server closed\n");
        return 0;
    }

    struct timeval tv;
    gettimeofday(&tv, 0);
    printf("received message at: %ld.%06d, message: %s\n",  tv.tv_sec, tv.tv_usec, recvBuf);

    return 0;
}