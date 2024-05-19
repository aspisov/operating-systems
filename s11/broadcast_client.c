#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BROADCAST_PORT 12345
#define BUFF_SIZE 256

int main() {
    int sockfd;
    struct sockaddr_in recv_addr;
    char buffer[BUFF_SIZE];
    socklen_t addr_len = sizeof(recv_addr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(BROADCAST_PORT);
    recv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&recv_addr, sizeof(recv_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        int n = recvfrom(sockfd, buffer, BUFF_SIZE, 0, (struct sockaddr *)&recv_addr, &addr_len);
        if (n < 0) {
            perror("Error in recvfrom");
        } else {
            buffer[n] = '\0';
            printf("Received message: %s", buffer);
        }
    }

    close(sockfd);
    return 0;
}
