#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_NAME "/tmp/fifo_processor_writer"
#define BUFFER_SIZE 5000

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int outputFd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (outputFd == -1) {
        perror("Ошибка при открытии выходного файла");
        exit(EXIT_FAILURE);
    }

    int fifoFd = open(FIFO_NAME, O_RDONLY);
    if (fifoFd == -1) {
        perror("Ошибка при открытии FIFO");

        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    int bytesRead;

    // Читаем данные из FIFO и записываем в выходной файл
    while ((bytesRead = read(fifoFd, buffer, BUFFER_SIZE)) > 0) {
        write(outputFd, buffer, bytesRead);
    }

    close(fifoFd);
    close(outputFd);

    return 0;
}
