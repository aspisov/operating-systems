#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_REQUEST "/tmp/fifo_request"
#define FIFO_RESPONSE "/tmp/fifo_response"
#define BUFFER_SIZE 5000 // Установка размера буфера в 5000 байт

void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Открытие входного файла для чтения
    int inputFd = open(argv[1], O_RDONLY);
    if (inputFd < 0) die("Opening input file");

    // Открытие FIFO для отправки запроса
    int fifoReqFd = open(FIFO_REQUEST, O_WRONLY);
    if (fifoReqFd < 0) die("Opening FIFO_REQUEST");

    // Чтение из входного файла и запись в FIFO_REQUEST
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead = read(inputFd, buffer, sizeof(buffer));
    if (bytesRead > 0) {
        if (write(fifoReqFd, buffer, bytesRead) != bytesRead) {
            die("Writing to FIFO_REQUEST");
        }
    }
    close(fifoReqFd);
    close(inputFd);

    // Открытие FIFO для получения ответа
    int fifoResFd = open(FIFO_RESPONSE, O_RDONLY);
    if (fifoResFd < 0) die("Opening FIFO_RESPONSE");

    // Открытие выходного файла для записи
    int outputFd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (outputFd < 0) die("Opening output file");

    // Чтение из FIFO_RESPONSE и запись в выходной файл
    while ((bytesRead = read(fifoResFd, buffer, sizeof(buffer))) > 0) {
        if (write(outputFd, buffer, bytesRead) != bytesRead) {
            die("Writing to output file");
        }
    }

    close(fifoResFd);
    close(outputFd);

    return EXIT_SUCCESS;
}
