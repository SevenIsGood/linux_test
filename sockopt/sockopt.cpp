#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {

    struct sockaddr_in addr;
    memset((void *)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket:");
        return 0;
    }

    int delay = 100;
    socklen_t len = sizeof(delay);

    if (getsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &delay, &len) < 0) {
        perror("getsockopt:");
        return 0;
    }

    printf("sock nodeay: %d\n", delay);

    return 0;
}