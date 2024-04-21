#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <vector>
#include <signal.h>
#include <fstream>
#include <ctime>
#include <pthread.h>
#include <cstring>  // Для strerror

// Определение структуры для передачи данных в поток
struct ThreadData {
    int startX, startY, dx, dy;
};

int M, N; // Размеры сада
std::ofstream logFile("garden_log.txt");
sem_t* semaphores[10][10]; // Предполагаемый максимальный размер сада

void initSemaphores() {
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            std::string semName = "/sem_" + std::to_string(i) + "_" + std::to_string(j);
            semaphores[i][j] = sem_open(semName.c_str(), O_CREAT | O_EXCL, 0666, 1);
            sem_unlink(semName.c_str());
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
    logFile << "Программа была прервана сигналом: " << sig << std::endl;
    destroySemaphores();
    logFile.close();
    exit(0);
}

void* gardenerWork(void* arg) {
    auto* data = (ThreadData*)arg;
    int x = data->startX, y = data->startY, dx = data->dx, dy = data->dy;
    while (true) {
        sem_wait(semaphores[x][y]);
        time_t now = time(nullptr);
        char* timeStr = ctime(&now);
        if (timeStr != nullptr) {
            logFile << "Садовник " << pthread_self() << " работает на участке (" << x << ", " << y << ") в " << timeStr;
        } else {
            logFile << "Ошибка при получении времени, errno: " << strerror(errno) << std::endl;
        }
        usleep(1000000); // Имитация работы в течение секунды
        sem_post(semaphores[x][y]);

        y += dy;
        if (y >= N || y < 0) {
            y -= dy;
            x += dx;
            if (x >= M || x < 0) break;
            dy = -dy;
        }
    }
    pthread_exit(nullptr);
}

int main(int argc, char** argv) {
    if (argc > 2) {
        M = std::stoi(argv[1]);
        N = std::stoi(argv[2]);
    } else {
        std::cout << "Введите размеры сада M и N: ";
        std::cin >> M >> N;
    }
    initSemaphores();
    signal(SIGINT, signalHandler);

    pthread_t threads[2];
    ThreadData data1 = {0, 0, 1, 1}, data2 = {M-1, N-1, -1, -1};

    pthread_create(&threads[0], NULL, gardenerWork, &data1);
    pthread_create(&threads[1], NULL, gardenerWork, &data2);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    destroySemaphores();
    logFile.close();
    return 0;
}
