#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define MAX 128

struct msg_buffer {
    long msg_type;
    char data[MAX];
} message;

int is_hex_char(char c) {
    return isdigit(c) || (tolower(c) >= 'a' && tolower(c) <= 'f');
}

int is_isolated_hex_sequence(const char *str, int start, int end) {
    // Проверяем, является ли символ до начала и после конца последовательности не-буквенным (или началом/концом строки)
    if ((start == 0 || !isalpha(str[start - 1])) &&
        (str[end] == '\0' || !isalpha(str[end]))) {
        return 1;
    }
    return 0;
}

void process_text(const char *str, int fd_out) {
    int start = -1;
    for (int i = 0; str[i] != '\0'; i++) {
        if (is_hex_char(str[i])) {
            if (start == -1) start = i; // Начало шестнадцатеричной последовательности
        } else {
            if (start != -1 && is_isolated_hex_sequence(str, start, i)) {
                // Выводим шестнадцатеричную последовательность, если она изолирована
                write(fd_out, &str[start], i - start);
                write(fd_out, "\n", 1);
                start = -1; // Сброс начала последовательности
            } else {
                start = -1; // Сброс начала последовательности, если текущая не изолирована
            }
        }
    }
    // Проверяем последнюю последовательность в строке
    if (start != -1 && is_isolated_hex_sequence(str, start, strlen(str))) {
        write(fd_out, &str[start], strlen(str) - start);
        write(fd_out, "\n", 1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <output_file_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fd_out = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out == -1) {
        perror("Failed to open the output file");
        exit(EXIT_FAILURE);
    }

    key_t key = ftok("queue", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);

    while (1) {
        if (msgrcv(msgid, &message, sizeof(message), 1, 0) == -1) {
            perror("Error receiving message");
            exit(EXIT_FAILURE);
        }
        if (strcmp(message.data, "EOF") == 0) break;
        process_text(message.data, fd_out);
    }

    close(fd_out); // Закрываем файл вывода
    msgctl(msgid, IPC_RMID, NULL); // Удаляем очередь сообщений

    return 0;
}