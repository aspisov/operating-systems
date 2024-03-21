#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MAX_TEXT 120
#define MSG_TYPE_DATA 1
#define MSG_TYPE_EOF 2

struct msg_buffer {
    long msg_type;
    char text[MAX_TEXT];
};

int is_hex_char(char c) {
    return (isdigit(c) || (tolower(c) >= 'a' && tolower(c) <= 'f'));
}

void process_hex_numbers(const char *str, char *result) {
    int count = 0, isNewNumber = 1;
    char numberBuffer[MAX_TEXT] = {0}; // Для сбора шестнадцатеричного числа
    char *resultPtr = result;
    resultPtr += sprintf(resultPtr, "Найдено шестнадцатеричных чисел: ");

    for (int i = 0; str[i] != '\0'; ++i) {
        if (is_hex_char(str[i])) {
            if (isNewNumber) {
                if (count > 0) {
                    *resultPtr++ = ',';
                }
                isNewNumber = 0;
                count++;
            }
            *resultPtr++ = str[i];
        } else {
            isNewNumber = 1;
        }
    }
    *resultPtr = '\0'; // Завершение строки
    sprintf(result, "%s. Всего: %d.", result, count); // Добавляем общее количество найденных чисел
}

int main() {
    key_t key = ftok("progfile", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct msg_buffer msg;

    while (1) {
        if (msgrcv(msgid, &msg, sizeof(msg), MSG_TYPE_DATA, 0) > 0) {
            if (strncmp(msg.text, "EOF", 3) == 0) {
                msg.msg_type = MSG_TYPE_EOF;
                msgsnd(msgid, &msg, sizeof(msg), 0); // Отправляем EOF обратно, сигнализируя о завершении обработки
                break;
            }
            char processedText[MAX_TEXT * 2] = {0}; // Буфер для обработанных данных
            process_hex_numbers(msg.text, processedText);

            // Подготавливаем и отправляем обработанный текст обратно
            msg.msg_type = MSG_TYPE_EOF;
            strncpy(msg.text, processedText, MAX_TEXT);
            msgsnd(msgid, &msg, strlen(msg.text) + 1, 0); // +1 для учета нулевого символа в конце строки
        }
    }

    return 0;
}
