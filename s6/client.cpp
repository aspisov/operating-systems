#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h> // для srand, rand
#include <time.h>   // для time
#include <unistd.h> // для sleep

#define SHM_KEY 0x1234 // Пример ключа для разделяемой памяти
#define N 10 // Размер массива данных

// Объявляем структуру SharedData
struct SharedData {
    int data[N];
    bool ready;
    bool finished;
};

// Прототип функции cleanUp для корректного завершения работы
void cleanUp(int shmid, SharedData* sharedData) {
    // Отсоединяем разделяемую память
    shmdt(sharedData);
}

int main() {
    // Получаем доступ к разделяемой памяти
    int shmid = shmget(SHM_KEY, sizeof(SharedData), 0644);
    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    // Присоединяем разделяемую память
    SharedData* sharedData = (SharedData*) shmat(shmid, NULL, 0);
    if (sharedData == (void*) -1) {
        perror("shmat");
        return 1;
    }

    // Инициализируем генератор случайных чисел
    srand(time(NULL));

    // Главный цикл клиента
    while (!sharedData->finished) {
        while (sharedData->ready) {
            sleep(1); // Ожидаем, пока сервер не обработает данные
        }

        for (int i = 0; i < N; i++) {
            sharedData->data[i] = rand() % 100; // Генерация случайных чисел
            std::cout << "Sent: " << sharedData->data[i] << std::endl;
        }
        sharedData->ready = true; // Устанавливаем флаг готовности данных
    }

    // Освобождаем ресурсы перед выходом
    cleanUp(shmid, sharedData);
    return 0;
}
