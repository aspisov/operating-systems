#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define MAX_TEXT 120
#define MSG_TYPE 1
#define EOF_TYPE 2

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
        msg.msg_type = MSG_TYPE;
        msgsnd(msgid, &msg, strlen(msg.text) + 1, 0);
    }
    strcpy(msg.text, "EOF");
    msgsnd(msgid, &msg, strlen(msg.text) + 1, MSG_TYPE);

    fclose(inputFile);

    FILE *outputFile = fopen(argv[2], "w");
    if (!outputFile) {
        perror("Ошибка открытия выходного файла");
        exit(EXIT_FAILURE);
    }

    while (1) {
        msgrcv(msgid, &msg, sizeof(msg), EOF_TYPE, 0);
        if (strcmp(msg.text, "EOF") == 0) {
            break;
        }
        fputs(msg.text, outputFile);
    }

    fclose(outputFile);
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}
