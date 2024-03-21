#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_TEXT 120
#define MSG_TYPE 1
#define EOF_TYPE 2

struct msg_buffer {
    long msg_type;
    char text[MAX_TEXT];
};

int is_hex_char(char c) {
    return (isdigit(c) || (tolower(c) >= 'a' && tolower(c) <= 'f'));
}

void process_hex_numbers(const char *str, char *result, int *hexCount) {
    int count = 0, isNewNumber = 1, index = 0;
    for (int i = 0; str[i] != '\0'; ++i) {
        if (is_hex_char(str[i])) {
            if (isNewNumber) {
                if (count > 0) result[index++] = ',';
                isNewNumber = 0;
                count++;
            }
            result[index++] = str[i];
        } else {
            isNewNumber = 1;
        }
    }
    result[index] = '\0';
    *hexCount = count;
}

int main() {
    key_t key = ftok("progfile", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct msg_buffer msg;

    while (1) {
        msgrcv(msgid, &msg, sizeof(msg), MSG_TYPE, 0);
        if (strcmp(msg.text, "EOF") == 0) {
            break;
        }
        int hexCount;
        char processedText[MAX_TEXT * 2] = {0};
        process_hex_numbers(msg.text, processedText, &hexCount);

        snprintf(msg.text, sizeof(msg.text), "%d шестнадцатеричных чисел: %s", hexCount, processedText);
        msg.msg_type = EOF_TYPE;
        msgsnd(msgid, &msg, strlen(msg.text) + 1, 0);
    }

    msg.msg_type = EOF_TYPE;
    strcpy(msg.text, "EOF");
    msgsnd(msgid, &msg, strlen(msg.text) + 1, 0);

    return 0;
}
