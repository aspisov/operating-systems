#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <vector>
#include <signal.h>

const int M = 5; // Количество строк в саду
const int N = 5; // Количество столбцов в саду

// Прототип функции
void destroySemaphores();

// Семафоры для управления доступом к каждому квадрату сада
sem_t* semaphores[M][N];

void signalHandler(int sig) {
    // Очистка при завершении программы
    destroySemaphores();
    std::cout << "Программа была прервана сигналом " << sig << std::endl;
    exit(0);
}

void initSemaphores() {
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            semaphores[i][j] = sem_open((std::string("SEM_") + std::to_string(i) + "_" + std::to_string(j)).c_str(), O_CREAT, 0666, 1);
        }
    }
    // Установка обработчика сигналов
    signal(SIGINT, signalHandler);
}

void destroySemaphores() {
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            sem_close(semaphores[i][j]);
            sem_unlink((std::string("SEM_") + std::to_string(i) + "_" + std::to_string(j)).c_str());
        }
    }
}

void gardenerWork(int startX, int startY, int endX, int endY, int dx, int dy) {
    int x = startX;
    int y = startY;
    while (x != endX || y != endY) {
        sem_wait(semaphores[x][y]);
        std::cout << "Садовник " << getpid() << " работает на участке (" << x << ", " << y << ")" << std::endl;
        usleep(500000); // Половина секунды задержки для визуализации
        sem_post(semaphores[x][y]);

        y += dy;
        if (y == N || y < 0) {
            y -= dy;
            x += dx;
            dy = -dy;
        }
        usleep(500000); // Задержка для имитации работы
    }
}

int main() {
    initSemaphores();

    pid_t pid = fork();
    if (pid == 0) {
        gardenerWork(0, 0, M - 1, N - 1, 1, 1);
    } else {
        gardenerWork(M - 1, N - 1, 0, 0, -1, -1);
        wait(NULL);
    }

    destroySemaphores();
    return 0;
}
