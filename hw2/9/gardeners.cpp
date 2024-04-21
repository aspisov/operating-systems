#include <iostream>
#include <vector>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fstream>
#include <ctime>
#include <iomanip>

struct ThreadData {
    int startX, startY, dx, dy, index;
};

int M, N;
std::vector<std::vector<sem_t*>> semaphores;  // Используем указатели на семафоры
std::vector<std::vector<char>> garden;
std::ofstream logFile("garden_log.txt");

void initSemaphores() {
    semaphores.resize(M, std::vector<sem_t*>(N));
    garden.resize(M, std::vector<char>(N, ' '));
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            semaphores[i][j] = sem_open(("/sem_" + std::to_string(i) + "_" + std::to_string(j)).c_str(), O_CREAT | O_EXCL, 0666, 1);
            sem_unlink(("/sem_" + std::to_string(i) + "_" + std::to_string(j)).c_str());  // Удалить имя из глобального пространства имен
        }
    }
}

void destroySemaphores() {
    for (auto &row : semaphores) {
        for (auto &sem : row) {
            sem_close(sem);
        }
    }
}

void printGarden() {
    for (const auto &row : garden) {
        for (char plot : row) {
            std::cout << plot << " ";
        }
        std::cout << "\n";
    }
}

void* gardenerWork(void* arg) {
    auto* data = (ThreadData*)arg;
    int x = data->startX, y = data->startY;
    while (true) {
        sem_wait(semaphores[x][y]);
        garden[x][y] = 'G' + data->index - 1;
        printGarden();
        usleep(1000000); // Имитация работы
        garden[x][y] = ' ';
        sem_post(semaphores[x][y]);

        y += data->dy;
        if (y >= N || y < 0) {
            y -= data->dy;
            x += data->dx;
            if (x >= M || x < 0) break;
            data->dy = -data->dy;
        }
    }
    pthread_exit(nullptr);
}

int main() {
    std::cout << "Enter dimensions of the garden M and N: ";
    std::cin >> M >> N;

    initSemaphores();
    logFile << "Semaphores initialized.\n";

    ThreadData data1 = {0, 0, 1, 1, 1}, data2 = {M-1, N-1, -1, -1, 2};
    pthread_t threads[2];
    pthread_create(&threads[0], nullptr, gardenerWork, &data1);
    pthread_create(&threads[1], nullptr, gardenerWork, &data2);

    pthread_join(threads[0], nullptr);
    pthread_join(threads[1], nullptr);

    destroySemaphores();
    logFile.close();
    return 0;
}
