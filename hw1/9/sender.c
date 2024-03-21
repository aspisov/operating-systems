#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define MAX_TEXT 120
#define MSG_TYPE_DATA 1
#define MSG_TYPE_EOF 2

struct msg_buffer {
    long msg_type;
    char text[MAX_TEXT];
};

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <input_file> <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    key_t key = ftok("progfile", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct msg_buffer msg;

    FILE *inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        perror("Ошибка открытия входного файла");
        exit(EXIT_FAILURE);
    }

    while (fgets(msg.text, MAX_TEXT, inputFile) != NULL) {
        msg.msg_type = MSG_TYPE_DATA;
        msgsnd(msgid, &msg, strlen(msg.text) + 1, 0);
    }
    strcpy(msg.text, "EOF");
    msg.msg_type = MSG_TYPE_DATA;
    msgsnd(msgid, &msg, strlen(msg.text) + 1, 0);

    fclose(inputFile);

    FILE *outputFile = fopen(argv[2], "w");
    if (!outputFile) {
        perror("Ошибка открытия выходного файла");
        exit(EXIT_FAILURE);
    }

    // Получение обработанных данных и запись их в выходной файл
    while (1) {
        msgrcv(msgid, &msg, sizeof(msg), MSG_TYPE_EOF, 0);
        if (strncmp(msg.text, "EOF", 3) == 0) {
            break;
        }
        fputs(msg.text, outputFile);
        fputs("\n", outputFile);  // Добавляем перенос строки для каждого сообщения
    }

    fclose(outputFile);
    msgctl(msgid, IPC_RMID, NULL); // Удаление очереди сообщений

    return 0;
}
