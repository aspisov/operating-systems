#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define M 8  // Number of rows
#define N 8  // Number of columns
#define PERCENT_OBSTACLES_MIN 10
#define PERCENT_OBSTACLES_MAX 30

typedef struct {
    int x;
    int y;
    int gardener_id;
    int action;  // 0: processing cell, 1: encountered obstacle
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

typedef struct {
    int socket;
    pthread_t thread;
    int type;  // 1: gardener, 2: monitor
} Client;

Client clients[100];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast_message(Message *msg) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (clients[i].type == 2) { // Only send to monitor clients
            send(clients[i].socket, msg, sizeof(Message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(int client_socket) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (clients[i].socket == client_socket) {
            clients[i] = clients[--client_count];
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void* handle_gardener_client(void* arg) {
    int client_socket = *(int*)arg;
    free(arg);
    Message msg;

    while (recv(client_socket, &msg, sizeof(msg), 0) > 0) {
        pthread_mutex_lock(&garden_mutex);
        if (msg.action == 0 && garden[msg.x][msg.y] == 0) {
            garden[msg.x][msg.y] = msg.gardener_id;
        }
        pthread_mutex_unlock(&garden_mutex);

        broadcast_message(&msg);
        send(client_socket, garden, sizeof(garden), 0);
    }

    remove_client(client_socket);
    close(client_socket);
    return NULL;
}

void* handle_monitor_client(void* arg) {
    int client_socket = *(int*)arg;
    free(arg);

    // Keep the connection open for the monitor client to receive updates
    while (1) {
        sleep(1);  // Keep the connection alive
    }

    remove_client(client_socket);
    close(client_socket);
    return NULL;
}

void add_client(int client_socket, int type) {
    pthread_mutex_lock(&clients_mutex);
    clients[client_count].socket = client_socket;
    clients[client_count].type = type;
    client_count++;
    pthread_mutex_unlock(&clients_mutex);
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

    if (listen(server_fd, 10) < 0) {
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

        char buffer[10];
        recv(new_socket, buffer, sizeof(buffer), 0);
        int client_type = atoi(buffer);

        add_client(new_socket, client_type);

        if (client_type == 1) {
            pthread_create(&thread, NULL, handle_gardener_client, client_socket);
        } else if (client_type == 2) {
            pthread_create(&thread, NULL, handle_monitor_client, client_socket);
        }
    }

    if (new_socket < 0) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    return 0;
}
