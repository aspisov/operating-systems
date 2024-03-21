#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define FIFO_TO_PROCESSOR "/tmp/fifo_to_processor"
#define FIFO_FROM_PROCESSOR "/tmp/fifo_from_processor"
#define BUFFER_SIZE 5000

int is_hex_char(char c) {
    return isdigit(c) || (tolower(c) >= 'a' && tolower(c) <= 'f');
}

void process_hex_data(const char *input, int len, char *output, int *count) {
    int outputIndex = 0, isNewNumber = 1;
    *count = 0;
    for (int i = 0; i < len; ++i) {
        if (is_hex_char(input[i])) {
            if (isNewNumber) {
                if (*count > 0) output[outputIndex++] = ',';
                isNewNumber = 0;
                (*count)++;
            }
            output[outputIndex++] = input[i];
        } else {
            isNewNumber = 1;
        }
    }
    output[outputIndex] = '\0';
}

int main() {
    int fifo_to = open(FIFO_TO_PROCESSOR, O_RDONLY);
    int fifo_from = open(FIFO_FROM_PROCESSOR, O_WRONLY);
    if (fifo_to == -1 || fifo_from == -1) {
        perror("Ошибка открытия FIFO");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE], processed[BUFFER_SIZE * 2];
    int count;

    ssize_t bytesRead = read(fifo_to, buffer, sizeof(buffer));
    if (bytesRead > 0) {
        process_hex_data(buffer, bytesRead, processed, &count);
        char countMsg[BUFFER_SIZE];
        snprintf(countMsg, sizeof(countMsg), "%d Чисел: %s", count, processed);
        write(fifo_from, countMsg, strlen(countMsg) + 1);
    }

    close(fifo_to);
    close(fifo_from);
    return EXIT_SUCCESS;
}
