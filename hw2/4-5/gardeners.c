#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/mman.h>

int M, N; // Размеры участка
float speed1, speed2; // Скорости садовников

// Инициализация массива семафоров
sem_t** init_semaphores() {
    sem_t** semaphores = malloc(M * N * sizeof(sem_t*));
    char sem_name[64];

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            snprintf(sem_name, sizeof(sem_name), "sem_%d_%d", i, j);
            semaphores[i * N + j] = sem_open(sem_name, O_CREAT | O_EXCL, 0666, 1);
            sem_unlink(sem_name);
        }
    }
    return semaphores;
}

// Удаление семафоров
void destroy_semaphores(sem_t** semaphores) {
    for (int i = 0; i < M * N; i++) {
        sem_close(semaphores[i]);
    }
    free(semaphores);
}

// Работа садовника
void gardener_work(int id, int startRow, int startCol, int dRow, int dCol, int isHorizontal, sem_t** semaphores, int* gardenLayout, float speed) {
    int i = startRow, j = startCol;
    while (i >= 0 && i < M && j >= 0 && j < N) {
        sem_wait(semaphores[i * N + j]);
        if (gardenLayout[i * N + j] == -1) {
            printf("Садовник %d наткнулся на препятствие на (%d,%d)\n", id, i, j);
        } else if (gardenLayout[i * N + j] == 1) {
            printf("Садовник %d наткнулся на уже обработанную клетку на (%d,%d)\n", id, i, j);
        } else {
            printf("Садовник %d обрабатывает клетку (%d,%d)\n", id, i, j);
            gardenLayout[i * N + j] = 1;
            usleep((int)(1000000 / speed));
        }
        sem_post(semaphores[i * N + j]);

        if (isHorizontal) {
            j += dCol;
            if (j == N || j == -1) {
                j -= dCol;
                i += dRow;
                dCol = -dCol; // Change direction
            }
        } else {
            i += dRow;
            if (i == M || i == -1) {
                i -= dRow;
                j += dCol;
                dRow = -dRow; // Change direction
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s M N speed1 speed2\n", argv[0]);
        return 1;
    }

    M = atoi(argv[1]);
    N = atoi(argv[2]);
    speed1 = atof(argv[3]);
    speed2 = atof(argv[4]);
    int obstaclePercentage = rand() % 16 + 15;

    int* gardenLayout = mmap(NULL, M * N * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            gardenLayout[i * N + j] = (rand() % 100 < obstaclePercentage) ? -1 : 0;
        }
    }

    sem_t** semaphores = init_semaphores();

    pid_t pid1 = fork();
    if (pid1 == 0) {
        gardener_work(1, 0, 0, 1, 1, 1, semaphores, gardenLayout, speed1);
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        gardener_work(2, M-1, N-1, -1, -1, 0, semaphores, gardenLayout, speed2);
        exit(0);
    }

    wait(NULL);
    wait(NULL);

    printf("Все доступные клетки обработаны.\n");
    destroy_semaphores(semaphores);
    munmap(gardenLayout, M * N * sizeof(int));

    return 0;
}
