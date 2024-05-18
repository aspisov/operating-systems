#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define M 8  // Number of rows
#define N 8  // Number of columns

typedef struct {
    int x;
    int y;
    int gardener_id;
    int action;  // 0: processing cell, 1: encountered obstacle
} Message;

void display_message(Message *msg) {
    if (msg->action == 0) {
        printf("Gardener %d processed cell (%d, %d)\n", msg->gardener_id, msg->x, msg->y);
    } else if (msg->action == 1) {
        printf("Gardener %d encountered obstacle at (%d, %d)\n", msg->gardener_id, msg->x, msg->y);
    }
}

int main(int argc, char const *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP> <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

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
    sprintf(buffer, "%d", 2);
    send(sock, buffer, sizeof(buffer), 0);

    Message msg;

    while (recv(sock, &msg, sizeof(msg), 0) > 0) {
        display_message(&msg);
    }

    close(sock);
    return 0;
}
