#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_TO_PROCESSOR "/tmp/fifo_to_processor"
#define FIFO_FROM_PROCESSOR "/tmp/fifo_from_processor"
#define BUFFER_SIZE 5000 // Установка размера буфера в 5000 байт

void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <input_file> <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Создание FIFO, если они еще не созданы. Можно опустить, если FIFO создаются заранее.
    if (mkfifo(FIFO_TO_PROCESSOR, 0666) == -1 || mkfifo(FIFO_FROM_PROCESSOR, 0666) == -1) {
        die("mkfifo");
    }

    // Открытие FIFO для записи
    int fifo_to = open(FIFO_TO_PROCESSOR, O_WRONLY);
    if (fifo_to == -1) die("Ошибка открытия FIFO_TO_PROCESSOR");

    // Открытие входного файла
    int inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1) die("Ошибка открытия входного файла");

    // Чтение из файла и запись в FIFO
    char buffer[BUFFER_SIZE];
    ssize_t numRead;
    while ((numRead = read(inputFd, buffer, sizeof(buffer))) > 0) {
        if (write(fifo_to, buffer, numRead) != numRead) die("Ошибка записи в FIFO_TO_PROCESSOR");
    }
    close(inputFd);
    close(fifo_to); // Важно закрыть, чтобы processor_fifo.c мог корректно обнаружить конец данных.

    // Открытие FIFO для чтения
    int fifo_from = open(FIFO_FROM_PROCESSOR, O_RDONLY);
    if (fifo_from == -1) die("Ошибка открытия FIFO_FROM_PROCESSOR");

    // Открытие выходного файла
    int outputFd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (outputFd == -1) die("Ошибка открытия выходного файла");

    // Чтение из FIFO и запись в выходной файл
    while ((numRead = read(fifo_from, buffer, sizeof(buffer))) > 0) {
        if (write(outputFd, buffer, numRead) != numRead) die("Ошибка записи в выходной файл");
    }
    close(outputFd);
    close(fifo_from);

    return EXIT_SUCCESS;
}
