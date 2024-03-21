#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_NAME "/tmp/fifo_reader_processor"
#define BUFFER_SIZE 5000

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1) {
        perror("Ошибка при открытии входного файла");
        exit(EXIT_FAILURE);
    }

    int fifoFd = open(FIFO_NAME, O_WRONLY);
    if (fifoFd == -1) {
        perror("Ошибка при открытии FIFO");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    int bytesRead = read(inputFd, buffer, BUFFER_SIZE);
    if (bytesRead > 0) {
        write(fifoFd, buffer, bytesRead);
    }

    close(inputFd);
    close(fifoFd);

    return 0;
}
