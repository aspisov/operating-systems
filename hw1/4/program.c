#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>

#define BUFFER_SIZE 5000

int is_hex_char(char c) {
    return (isdigit(c) || (tolower(c) >= 'a' && tolower(c) <= 'f'));
}

void process_hex_numbers(const char *str, char *result, int *count) {
    int index = 0, hex_started = 0;
    *count = 0;

    for (int i = 0; i < BUFFER_SIZE && str[i] != '\0'; ++i) {
        if (is_hex_char(str[i])) {
            if (!hex_started) { // Начало нового шестнадцатеричного числа
                hex_started = 1;
                if (*count > 0) result[index++] = ','; // Добавляем запятую между числами
            }
            result[index++] = str[i];
        } else {
            if (hex_started) {
                hex_started = 0; // Конец шестнадцатеричного числа
                (*count)++;
            }
        }
    }

    if (hex_started) (*count)++; // Если строка заканчивается шестнадцатеричным числом
    result[index] = '\0'; // Завершаем строку
}

int main(int argc, char *argv[]) {
    int pipe_fd[2];
    pid_t pid;
    char buf[BUFFER_SIZE] = {0};
    char processed[BUFFER_SIZE] = {0};
    int count = 0;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Дочерний процесс
        close(pipe_fd[1]);
        read(pipe_fd[0], buf, BUFFER_SIZE);
        process_hex_numbers(buf, processed, &count);
        printf("Found %d hex numbers: %s\n", count, processed);
        close(pipe_fd[0]);
        exit(EXIT_SUCCESS);
    } else { // Родительский процесс
        close(pipe_fd[0]);
        int fd = open(argv[1], O_RDONLY);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        ssize_t num_read = read(fd, buf, BUFFER_SIZE);
        if (num_read > 0) {
            write(pipe_fd[1], buf, num_read);
        } else {
            write(pipe_fd[1], buf, 1); // Передаем хотя бы один байт, чтобы избежать блокировки
        }

        close(fd);
        close(pipe_fd[1]);
        wait(NULL);
    }

    return EXIT_SUCCESS;
}