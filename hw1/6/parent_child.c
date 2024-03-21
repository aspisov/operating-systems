#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFFER_SIZE 5000

int is_hex_char(char c) {
    return (isdigit(c) || (tolower(c) >= 'a' && tolower(c) <= 'f'));
}

void process_hex_numbers(const char *str, char *result, int *count) {
    int index = 0;
    *count = 0;
    int isNewNumber = 1;

    for (int i = 0; i < BUFFER_SIZE && str[i] != '\0'; ++i) {
        if (is_hex_char(str[i])) {
            if (isNewNumber) {
                if (*count > 0) result[index++] = ',';
                isNewNumber = 0;
                (*count)++;
            }
            result[index++] = str[i];
        } else {
            isNewNumber = 1;
        }
    }
    result[index] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int pipe_fd[2], pipe_fd_back[2];
    pid_t pid;
    char buffer[BUFFER_SIZE] = {0};
    char processed[BUFFER_SIZE * 2] = {0};
    int count = 0;

    if (pipe(pipe_fd) == -1 || pipe(pipe_fd_back) == -1) {
        perror("Ошибка создания канала");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("Ошибка создания процесса");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Дочерний процесс
        close(pipe_fd[1]);
        close(pipe_fd_back[0]);

        read(pipe_fd[0], buffer, BUFFER_SIZE);
        process_hex_numbers(buffer, processed, &count);

        char message[BUFFER_SIZE + 50];
        sprintf(message, "%d Шестнадцатеричных чисел: %s", count, processed);
        write(pipe_fd_back[1], message, strlen(message) + 1);

        close(pipe_fd[0]);
        close(pipe_fd_back[1]);
        exit(EXIT_SUCCESS);
    } else { // Родительский процесс
        close(pipe_fd[0]);
        close(pipe_fd_back[1]);

        int inputFd = open(argv[1], O_RDONLY);
        if (inputFd == -1) {
            perror("Ошибка открытия входного файла");
            exit(EXIT_FAILURE);
        }

        ssize_t num_read = read(inputFd, buffer, BUFFER_SIZE);
        if (num_read > 0) {
            write(pipe_fd[1], buffer, num_read);
        } else {
            write(pipe_fd[1], buffer, 1); // Передаем хотя бы один байт, чтобы избежать блокировки
        }
        close(inputFd);
        close(pipe_fd[1]);

        wait(NULL); // Ожидание завершения дочернего процесса

        int outputFd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (outputFd == -1) {
            perror("Ошибка открытия выходного файла");
            exit(EXIT_FAILURE);
        }

        read(pipe_fd_back[0], processed, sizeof(processed));
        write(outputFd, processed, strlen(processed));

        close(outputFd);
        close(pipe_fd_back[0]);
    }

    return EXIT_SUCCESS;
}
