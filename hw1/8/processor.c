#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_REQUEST "/tmp/fifo_request"
#define FIFO_RESPONSE "/tmp/fifo_response"
#define BUFFER_SIZE 5000

int is_hex_char(char c) {
    return isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

void process_hex_data(const char *data, char *processed, int *count, ssize_t size) {
    int isNewNumber = 1, index = 0;
    *count = 0;
    for (ssize_t i = 0; i < size; ++i) {
        if (is_hex_char(data[i])) {
            if (isNewNumber) {
                if (*count > 0) processed[index++] = ',';
                isNewNumber = 0;
                (*count)++;
            }
            processed[index++] = data[i];
        } else {
            isNewNumber = 1;
        }
    }
    processed[index] = '\0';
}

int main() {
    mkfifo(FIFO_REQUEST, 0666);
    mkfifo(FIFO_RESPONSE, 0666);

    int fifoReqFd = open(FIFO_REQUEST, O_RDONLY);
    int fifoResFd = open(FIFO_RESPONSE, O_WRONLY);
    if (fifoReqFd < 0 || fifoResFd < 0) {
        perror("Ошибка при открытии FIFO");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE], processed[BUFFER_SIZE * 2];
    ssize_t bytesRead;
    int count;

    while ((bytesRead = read(fifoReqFd, buffer, sizeof(buffer))) > 0) {
        process_hex_data(buffer, processed, &count, bytesRead);
        char countMsg[BUFFER_SIZE + 100];
        snprintf(countMsg, sizeof(countMsg), "%d чисел: %s", count, processed);
        write(fifoResFd, countMsg, strlen(countMsg) + 1);
    }

    close(fifoReqFd);
    close(fifoResFd);
    return EXIT_SUCCESS;
}
