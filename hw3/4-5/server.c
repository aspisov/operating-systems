#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define M 10  // Number of rows
#define N 10 // Number of columns
#define PERCENT_OBSTACLES_MIN 10
#define PERCENT_OBSTACLES_MAX 30

typedef struct {
    int x;
    int y;
    int gardener_id;
} Message;

int garden[M][N];
pthread_mutex_t garden_mutex = PTHREAD_MUTEX_INITIALIZER;

void initialize_garden() {
    srand(time(0));
    int obstacles = PERCENT_OBSTACLES_MIN + rand() % (PERCENT_OBSTACLES_MAX - PERCENT_OBSTACLES_MIN + 1);
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            garden[i][j] = (rand() % 100 < obstacles) ? -1 : 0;
        }
    }
}

void print_garden() {
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

void* handle_client(void* arg) {
    int client_socket = *(int*)arg;
    free(arg);
    Message msg;

    while (recv(client_socket, &msg, sizeof(msg), 0) > 0) {
        pthread_mutex_lock(&garden_mutex);
        if (garden[msg.x][msg.y] == 0) {
            garden[msg.x][msg.y] = msg.gardener_id;
        }
        pthread_mutex_unlock(&garden_mutex);

        send(client_socket, garden, sizeof(garden), 0);
    }

    close(client_socket);
    return NULL;
}

int main() {
    initialize_garden();

    int server_fd, new_socket, *client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) >= 0) {
        printf("New connection accepted\n");
        pthread_t thread;
        client_socket = malloc(sizeof(int));
        *client_socket = new_socket;
        pthread_create(&thread, NULL, handle_client, client_socket);
    }

    if (new_socket < 0) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    return 0;
}
