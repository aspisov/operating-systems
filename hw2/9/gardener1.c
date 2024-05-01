#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define SEM_KEY 1234 // Ключ для семафоров

int main() {
    // Получение доступа к семафору
    int sem_id = semget(SEM_KEY, 1, 0);
    if (sem_id == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    // Заблокировать семафор
    struct sembuf wait_op = {0, -1, 0};
    if (semop(sem_id, &wait_op, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }

    printf("Садовник 1 начинает работу.\n");

    // Разблокировать семафор
    struct sembuf release_op = {0, 1, 0};
    if (semop(sem_id, &release_op, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }

    return 0;
}
