#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile int bit = 0; // Текущий принимаемый бит
volatile long long receivedNumber = 0; // Принятое число
volatile int receivedBits = 0; // Количество принятых битов
int senderPID; // PID передатчика

void handleSIGUSR1(int signum) {
    receivedNumber = (receivedNumber << 1) | bit;
    receivedBits++;
    kill(senderPID, SIGUSR1); // Подтверждение приема бита
}

void handleSIGUSR2(int signum) {
    bit = 1; // Смена бита на 1 для следующего сигнала
    receivedNumber = (receivedNumber << 1) | bit;
    receivedBits++;
    bit = 0; // Сброс бита на 0
    kill(senderPID, SIGUSR2); // Подтверждение приема бита
}

void handleSIGINT(int signum) {
    printf("Received number: %lld\n", receivedNumber);
    exit(0);
}

int main() {
    printf("Receiver PID: %d\n", getpid());
    printf("Enter sender PID: ");
    scanf("%d", &senderPID);

    struct sigaction sa;
    sa.sa_handler = &handleSIGUSR1;
    sigaction(SIGUSR1, &sa, NULL);

    sa.sa_handler = &handleSIGUSR2;
    sigaction(SIGUSR2, &sa, NULL);

    sa.sa_handler = &handleSIGINT;
    sigaction(SIGINT, &sa, NULL);

    while (1) {
        pause(); // Ожидание сигнала
    }

    return 0;
}
