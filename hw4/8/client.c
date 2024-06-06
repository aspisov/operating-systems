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

typedef struct {
    int x;
    int y;
    int gardener_id;
    int action;  // 0: processing cell, 1: encountered obstacle
} Message;

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

void gardener1_work(int sockfd, struct sockaddr_in* servaddr, socklen_t addr_len) {
    Message msg;
    msg.gardener_id = 1;
    msg.x = 0;
    msg.y = 0;
    int direction = 1; // 1 for moving right, -1 for moving left

    int garden[M][N];

    while (msg.x < M) {
        if (garden[msg.x][msg.y] == -1) {
            msg.action = 1;
        } else {
            msg.action = 0;
        }
        sendto(sockfd, &msg, sizeof(msg), 0, (const struct sockaddr *) servaddr, addr_len);
        recvfrom(sockfd, garden, sizeof(garden), 0, (struct sockaddr *) servaddr, &addr_len);

        print_garden(garden);

        if (direction == 1 && msg.y + 1 < N) {
            msg.y++;
        } else if (direction == -1 && msg.y - 1 >= 0) {
            msg.y--;
        } else {
            msg.x++;
            direction *= -1;
        }

        usleep(100000);
    }
}

void gardener2_work(int sockfd, struct sockaddr_in* servaddr, socklen_t addr_len) {
    Message msg;
    msg.gardener_id = 2;
    msg.x = M - 1;
    msg.y = N - 1;
    int direction = -1; // -1 for moving left, 1 for moving right

    int garden[M][N];

    while (msg.y >= 0) {
        if (garden[msg.x][msg.y] == -1) {
            msg.action = 1;
        } else {
            msg.action = 0;
        }
        sendto(sockfd, &msg, sizeof(msg), 0, (const struct sockaddr *) servaddr, addr_len);
        recvfrom(sockfd, garden, sizeof(garden), 0, (struct sockaddr *) servaddr, &addr_len);

        print_garden(garden);

        if (direction == -1 && msg.x - 1 >= 0) {
            msg.x--;
        } else if (direction == 1 && msg.x + 1 < M) {
            msg.x++;
        } else {
            msg.y--;
            direction *= -1;
        }

        usleep(100000);
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

    gardener1_work(sockfd, &servaddr, addr_len);
    gardener2_work(sockfd, &servaddr, addr_len);

    close(sockfd);
    return 0;
}
