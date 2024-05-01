#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#define SEM_NAME "/garden_semaphore"

int main() {
    // Создание и открытие именованного семафора POSIX
    sem_t *sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    printf("Сад успешно инициализирован.\n");

    // Закрытие семафора
    sem_close(sem);

    // Удаление именованного семафора
    sem_unlink(SEM_NAME);

    return 0;
}
