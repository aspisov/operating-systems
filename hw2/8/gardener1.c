// gardener1.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <errno.h>

#define SHM_KEY 1234
#define SEM_KEY 5678
#define NUM_ROWS 5
#define NUM_COLS 5

typedef struct {
    int is_processed;
} Cell;

int shmid;
int semid;
Cell *garden;

void init_shared_memory() {
    if ((shmid = shmget(SHM_KEY, NUM_ROWS * NUM_COLS * sizeof(Cell), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((garden = (Cell *)shmat(shmid, NULL, 0)) == (Cell *) -1) {
        perror("shmat");
        exit(1);
    }
}

void destroy_shared_memory() {
    if (shmdt(garden) == -1) {
        perror("shmdt");
        exit(1);
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(1);
    }
}

void init_semaphores() {
    if ((semid = semget(SEM_KEY, 1, IPC_CREAT | 0666)) < 0) {
        perror("semget");
        exit(1);
    }

    if (semctl(semid, 0, SETVAL, 1) < 0) {
        perror("semctl");
        exit(1);
    }
}

void destroy_semaphores() {
    if (semctl(semid, 0, IPC_RMID) < 0) {
        perror("semctl");
        exit(1);
    }
}

void wait_semaphore() {
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;

    if (semop(semid, &sb, 1) < 0) {
        perror("semop");
        exit(1);
    }
}

void release_semaphore() {
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = SEM_UNDO;

    if (semop(semid, &sb, 1) < 0) {
        perror("semop");
        exit(1);
    }
}

void gardener_work(int id) {
    for (int i = 0; i < NUM_ROWS; ++i) {
        for (int j = 0; j < NUM_COLS; ++j) {
            wait_semaphore();
            if (!garden[i * NUM_COLS + j].is_processed) {
                printf("Садовник %d обрабатывает клетку (%d,%d)\n", id, i, j);
                garden[i * NUM_COLS + j].is_processed = 1;
            }
            release_semaphore();
        }
    }
}

int main() {
    init_shared_memory();
    init_semaphores();

    gardener_work(1);

    destroy_semaphores();
    destroy_shared_memory();

    printf("Программа садовника 1 завершила работу.\n");

    return 0;
}
