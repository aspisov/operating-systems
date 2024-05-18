#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define PORT 8080
#define M 10  // Number of rows
#define N 10  // Number of columns
#define PERCENT_OBSTACLES_MIN 10
#define PERCENT_OBSTACLES_MAX 30

typedef struct {
    int x;
    int y;
    int gardener_id;
    int action;  // 0: processing cell, 1: encountered obstacle, 2: shutdown
} Message;

int garden[M][N];
pthread_mutex_t garden_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int socket;
    pthread_t thread;
    int type;  // 1: gardener, 2: monitor
} Client;

Client clients[100];
int client_count = 0;
int server_running = 1;

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

void broadcast_message(Message *msg) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (send(clients[i].socket, msg, sizeof(Message), 0) < 0) {
            perror("Error broadcasting message");
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

void* handle_client(void* arg) {
    int client_socket = *(int*)arg;
    free(arg);
    Message msg;

    while (recv(client_socket, &msg, sizeof(msg), 0) > 0) {
        if (msg.action == 0 && garden[msg.x][msg.y] == 0) {
            pthread_mutex_lock(&garden_mutex);
            garden[msg.x][msg.y] = msg.gardener_id;
            pthread_mutex_unlock(&garden_mutex);
        }
        broadcast_message(&msg);
        if (send(client_socket, garden, sizeof(garden), 0) < 0) {
            perror("Error sending garden state");
        }
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

void handle_shutdown(int sig) {
    server_running = 0;

    Message shutdown_msg = { .action = 2 };
    broadcast_message(&shutdown_msg);

    for (int i = 0; i < client_count; i++) {
        close(clients[i].socket);
    }

    exit(0);
}

int main() {
    signal(SIGINT, handle_shutdown);

    initialize_garden();

    int server_fd, new_socket, *client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("Listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (server_running && (new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) >= 0) {
        printf("New connection accepted\n");

        pthread_t thread;
        client_socket = malloc(sizeof(int));
        *client_socket = new_socket;

        char buffer[10];
        recv(new_socket, buffer, sizeof(buffer), 0);
        int client_type = atoi(buffer);

        printf("Client type: %d\n", client_type);

        add_client(new_socket, client_type);
        pthread_create(&thread, NULL, handle_client, client_socket);
    }

    if (new_socket < 0) {
        perror("Accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    return 0;
}
