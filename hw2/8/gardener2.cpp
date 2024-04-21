#include <iostream>
#include <fstream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <cstring>

struct Garden {
    char plots[10][10];  // Максимальный предполагаемый размер сада
};

int main() {
    std::ifstream file("input.txt");
    int M, N;
    file >> M >> N;  // Считывание размеров сада
    file.close();

    if (M > 10 || N > 10) {
        std::cerr << "Size is too large." << std::endl;
        return 1;
    }

    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(Garden), 0666);
    auto* garden = (Garden*) shmat(shmid, (void*)0, 0);

    int semid = semget(key, 1, 0666);
    struct sembuf p = {-1, 0, SEM_UNDO}; // P-операция
    struct sembuf v = {1, 0, SEM_UNDO};  // V-операция

    std::ofstream logFile("garden_log.txt", std::ios::app);  // Открыть файл лога для добавления данных

    while (true) {
        semop(semid, &p, 1); // Захват семафора
        garden->plots[M/2][N/2 + 1] = 'H'; // Садовник работает в соседнем месте
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                logFile << garden->plots[i][j] << " ";
            }
            logFile << "\n";
        }
        logFile << "\n";
        sleep(1); // Имитация работы
        garden->plots[M/2][N/2 + 1] = ' ';
        semop(semid, &v, 1); // Освобождение семафора
        sleep(1); // Пауза
    }

    shmdt(garden); // Отсоединение разделяемой памяти
    return 0;
}
