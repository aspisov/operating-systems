//
// Created by Dmitry on 3/14/24.
//
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX 128

struct msg_buffer {
    long msg_type;
    char data[MAX];
} message;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Failed to open input file");
        return 1;
    }

    key_t key = ftok("hex_detector", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    message.msg_type = 1;

    ssize_t bytes_read;
    while ((bytes_read = read(fd, message.data, MAX - 1)) > 0) {
        message.data[bytes_read] = '\0'; // Ensure null-termination
        msgsnd(msgid, &message, sizeof(message), 0);
    }

    // Send EOF message
    strcpy(message.data, "EOF");
    msgsnd(msgid, &message, sizeof(message), 0);

    close(fd);
    return 0;
}
