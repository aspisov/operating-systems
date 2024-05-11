#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[BUF_SIZE] = {0};

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server IP> <server Port>\n", argv[0]);
        return 1;
    }

    // Create the socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return 1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return 1;
    }

    // Read messages from the server
    while(1) {
        valread = read(sock, buffer, BUF_SIZE);
        if (valread > 0) {
            printf("Received: %s\n", buffer);
            if (strcmp(buffer, "The End") == 0) break;  // Exit loop if "The End" is received
            memset(buffer, 0, BUF_SIZE);  // Clear the buffer
        }
    }

    close(sock);
    return 0;
}
