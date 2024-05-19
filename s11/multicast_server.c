#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MULTICAST_PORT 12345
#define MULTICAST_GROUP "239.255.255.250"
#define BUFF_SIZE 256

int main() {
    int sockfd;
    struct sockaddr_in multicast_addr;
    char message[BUFF_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_port = htons(MULTICAST_PORT);
    multicast_addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);

    while (1) {
        printf("Enter message to multicast: ");
        fgets(message, BUFF_SIZE, stdin);

        if (sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&multicast_addr, sizeof(multicast_addr)) < 0) {
            perror("Error in sendto");
        }
    }

    close(sockfd);
    return 0;
}
