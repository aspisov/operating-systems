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

    // Размер сада должен быть в пределах допустимого массива
    if (M > 10 || N > 10) {
        std::cerr << "Size is too large." << std::endl;
        return 1;
    }

    key_t key = ftok("shmfile",65);
    int shmid = shmget(key, sizeof(Garden), 0666|IPC_CREAT);
    auto* garden = (Garden*) shmat(shmid, (void*)0, 0);

    // Инициализация сада
    memset(garden->plots, ' ', sizeof(garden->plots));

    int semid = semget(key, 1, 0666|IPC_CREAT);
    semctl(semid, 0, SETVAL, 1);

    struct sembuf p = {-1, 0, SEM_UNDO};
    struct sembuf v = {1, 0, SEM_UNDO};

    std::ofstream logFile("garden_log.txt"); // Файл для записи лога

    while (true) {
        semop(semid, &p, 1);
        garden->plots[M/2][N/2] = 'G'; // Имитация работы в центре сада
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                logFile << garden->plots[i][j] << " ";
            }
            logFile << "\n";
        }
        logFile << "\n";
        sleep(1);
        garden->plots[M/2][N/2] = ' ';
        semop(semid, &v, 1);
        sleep(1);
    }

    shmdt(garden);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    logFile.close();
    return 0;
}
