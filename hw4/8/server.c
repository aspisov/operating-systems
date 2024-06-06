#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define M 8  // Number of rows
#define N 8  // Number of columns
#define PORT 8080
#define MAXLINE 1024

int garden[M][N];
pthread_mutex_t garden_mutex;

typedef struct {
    int x;
    int y;
    int gardener_id;
    int action;  // 0: processing cell, 1: encountered obstacle
} Message;

void initialize_garden() {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            garden[i][j] = 0;
        }
    }
}

void* handle_client(void* arg) {
    int sockfd = *(int*)arg;
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    Message msg;

    while (recvfrom(sockfd, &msg, sizeof(msg), 0, (struct sockaddr *) &cliaddr, &len) > 0) {
        pthread_mutex_lock(&garden_mutex);
        if (msg.action == 0 && garden[msg.x][msg.y] == 0) {
            garden[msg.x][msg.y] = msg.gardener_id;
        }
        pthread_mutex_unlock(&garden_mutex);

        sendto(sockfd, garden, sizeof(garden), 0, (const struct sockaddr *) &cliaddr, len);
    }

    return NULL;
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    pthread_mutex_init(&garden_mutex, NULL);
    initialize_garden();

    printf("Server listening on port %d\n", PORT);

    pthread_t thread;
    pthread_create(&thread, NULL, handle_client, &sockfd);
    pthread_join(thread, NULL);

    close(sockfd);
    pthread_mutex_destroy(&garden_mutex);
    return 0;
}
