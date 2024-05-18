#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define M 10  // Number of rows
#define N 10  // Number of columns

typedef struct {
    int x;
    int y;
    int gardener_id;
    int action;  // 0: processing cell, 1: encountered obstacle, 2: shutdown
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

void gardener1_work(int client_socket, int gardener_id) {
    Message msg;
    msg.gardener_id = gardener_id;
    msg.x = 0;
    msg.y = 0;
    int direction = 1; // 1 for moving right, -1 for moving left

    int garden[M][N];

    while (1) {
        if (recv(client_socket, &msg, sizeof(msg), 0) <= 0) {
            break;
        }

        if (msg.action == 2) {
            printf("Server is shutting down. Exiting...\n");
            break;
        }

        if (garden[msg.x][msg.y] == -1) {
            msg.action = 1;
        } else {
            msg.action = 0;
        }
        send(client_socket, &msg, sizeof(msg), 0);
        recv(client_socket, garden, sizeof(garden), 0);

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

void gardener2_work(int client_socket, int gardener_id) {
    Message msg;
    msg.gardener_id = gardener_id;
    msg.x = M - 1;
    msg.y = N - 1;
    int direction = -1; // -1 for moving left, 1 for moving right

    int garden[M][N];

    while (1) {
        if (recv(client_socket, &msg, sizeof(msg), 0) <= 0) {
            break;
        }

        if (msg.action == 2) {
            printf("Server is shutting down. Exiting...\n");
            break;
        }

        if (garden[msg.x][msg.y] == -1) {
            msg.action = 1;
        } else {
            msg.action = 0;
        }
        send(client_socket, &msg, sizeof(msg), 0);
        recv(client_socket, garden, sizeof(garden), 0);

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

int main(int argc, char const *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <IP> <PORT> <Gardener ID>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);
    int gardener_id = atoi(argv[3]);

    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    printf("Connected to server\n");

    char buffer[10];
    sprintf(buffer, "%d", 1);
    send(sock, buffer, sizeof(buffer), 0);

    if (gardener_id == 1) {
        gardener1_work(sock, gardener_id);
    } else if (gardener_id == 2) {
        gardener2_work(sock, gardener_id);
    }

    close(sock);
    return 0;
}
