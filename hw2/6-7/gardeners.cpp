#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <vector>
#include <signal.h>
#include <fstream>
#include <ctime>
#include <string>

// Глобальные переменные для размеров сада
int M, N;

// Файл для логирования
std::ofstream logFile("garden_log.txt");

// Семафоры для контроля доступа к каждому квадрату
sem_t* semaphores[10][10];  // Максимальный размер массива, предполагаемый для простоты

void initSemaphores() {
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            std::string semName = "/sem_" + std::to_string(i) + "_" + std::to_string(j);
            semaphores[i][j] = sem_open(semName.c_str(), O_CREAT | O_EXCL, 0666, 1);
            sem_unlink(semName.c_str());  // Удаляем имя, чтобы предотвратить утечку ресурсов
        }
    }
}

void destroySemaphores() {
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            sem_close(semaphores[i][j]);
        }
    }
}

void signalHandler(int sig) {
    logFile << "Программа была прервана сигналом " << sig << std::endl;
    destroySemaphores();
    logFile.close();
    exit(0);
}

void gardenerWork(int startX, int startY, int dx, int dy) {
    int x = startX, y = startY;
    while (true) {
        sem_wait(semaphores[x][y]);
        time_t now = time(nullptr);
        logFile << "Садовник " << getpid() << " работает на участке (" << x << ", " << y << ") в " << ctime(&now);
        usleep(1000000);  // Имитация работы в течение секунды
        sem_post(semaphores[x][y]);

        // Перемещение в следующий ряд или столбец
        y += dy;
        if (y >= N || y < 0) {
            y -= dy;  // Возвращение в пределы сада
            x += dx;
            if (x >= M || x < 0) {
                break;  // Выход из цикла, если достигнут край сада
            }
            dy = -dy;  // Меняем направление движения
        }
    }
}

int main() {
    std::cout << "Введите размеры сада M и N: ";
    std::cin >> M >> N;
    initSemaphores();
    signal(SIGINT, signalHandler);

    pid_t pid = fork();
    if (pid == 0) {
        gardenerWork(0, 0, 1, 1);
    } else {
        gardenerWork(M - 1, N - 1, -1, -1);
        wait(NULL);
    }

    destroySemaphores();
    logFile.close();
    return 0;
}
