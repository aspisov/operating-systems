#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int receiverPID; // PID приемника
volatile int ack = 0; // Подтверждение получения

void handleSIGUSR1(int signum) {
    ack = 1; // Подтверждение получения бита 0
}

void handleSIGUSR2(int signum) {
    ack = 1; // Подтверждение получения бита 1
}

void sendBit(int bit) {
    if (bit == 0) {
        kill(receiverPID, SIGUSR1);
    } else {
        kill(receiverPID, SIGUSR2);
    }

    // Ожидание подтверждения
    while (ack == 0) {
        pause();
    }
    ack = 0; // Сброс подтверждения для следующего бита
}

int main() {
    printf("Sender PID: %d\n", getpid());
    printf("Enter receiver PID: ");
    scanf("%d", &receiverPID);

    struct sigaction sa;
    sa.sa_handler = &handleSIGUSR1;
    sigaction(SIGUSR1, &sa, NULL);

    sa.sa_handler = &handleSIGUSR2;
    sigaction(SIGUSR2, &sa, NULL);

    long long number;
    printf("Enter a number: ");
    scanf("%lld", &number);

    // Отправка битов
    for (int i = sizeof(number) * 8 - 1; i >= 0; i--) {
        int bit = (number >> i) & 1;
        sendBit(bit);
    }

    // Завершение передачи можно организовать, например, отправкой специального сочетания сигналов
    // или использованием SIGINT для прерывания приемника после передачи последнего бита.

    return 0;
}
