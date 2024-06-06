#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define M 8  // Number of rows
#define N 8  // Number of columns
#define PORT 8080
#define MAXLINE 1024

void clear_terminal() {
    printf("\033[H\033[J");
}

void print_garden(int garden[M][N]) {
    clear_terminal();
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            if (garden[i][j] == -1) {
                printf("XX ");
            } else if (garden[i][j] == 1) {
                printf("G1 ");
            } else if (garden[i][j] == 2) {
                printf("G2 ");
            } else {
                printf("__ ");
            }
        }
        printf("\n");
    }
}

void monitor_garden(int sockfd, struct sockaddr_in* servaddr, socklen_t addr_len) {
    int garden[M][N];
    while (1) {
        recvfrom(sockfd, garden, sizeof(garden), 0, (struct sockaddr *) servaddr, &addr_len);
        print_garden(garden);
        usleep(500000);  // Refresh rate of the monitor
    }
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    socklen_t addr_len = sizeof(servaddr);

    monitor_garden(sockfd, &servaddr, addr_len);

    close(sockfd);
    return 0;
}
