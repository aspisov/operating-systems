#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define INPUT_FIFO_NAME "/tmp/fifo_reader_processor"
#define OUTPUT_FIFO_NAME "/tmp/fifo_processor_writer"
#define BUFFER_SIZE 5000

// Функция проверяет, является ли символ шестнадцатеричной цифрой
int is_hex_digit(char c) {
    return (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
}

// Функция для обработки строки: поиск шестнадцатеричных чисел и подсчет их количества
void process_data(const char *input, char *output, int *count) {
    int i = 0, num_count = 0, in_number = 0;
    while (input[i] != '\0' && i < BUFFER_SIZE) {
        if (is_hex_digit(input[i])) {
            if (!in_number) { // Начало нового числа
                in_number = 1;
                num_count++;
                if (num_count > 1) strcat(output, ", "); // Добавляем разделитель
            }
            strncat(output, &input[i], 1); // Добавляем символ к текущему числу
        } else {
            in_number = 0; // Конец текущего числа
        }
        i++;
    }
    *count = num_count; // Обновляем общее количество чисел
}

int main() {
    int inputFd = open(INPUT_FIFO_NAME, O_RDONLY);
    if (inputFd < 0) {
        perror("Ошибка открытия входного FIFO");
        exit(EXIT_FAILURE);
    }

    int outputFd = open(OUTPUT_FIFO_NAME, O_WRONLY);
    if (outputFd < 0) {
        perror("Ошибка открытия выходного FIFO");
        exit(EXIT_FAILURE);
    }

    char input[BUFFER_SIZE] = {0};
    char output[BUFFER_SIZE * 2] = "Обработанные данные: ";
    int bytesRead = read(inputFd, input, BUFFER_SIZE);
    if (bytesRead > 0) {
        int count = 0;
        process_data(input, output + strlen(output), &count); // Обработка входных данных
        sprintf(output + strlen(output), ". Всего шестнадцатеричных чисел: %d.", count);
        write(outputFd, output, strlen(output) + 1); // Отправка обработанных данных
    }

    close(inputFd);
    close(outputFd);

    return 0;
}
