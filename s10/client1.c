#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define PORT 8080  // Define the port number directly

int main() {
    struct sockaddr_in serv_addr;
    int sock = 0;
    char *message = "Hello from client";
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    send(sock, message, strlen(message), 0);
    printf("Hello message sent\n");
    // continue sending messages
    return 0;
}
