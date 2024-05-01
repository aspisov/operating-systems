// initialize_garden.c

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_KEY 1234
#define NUM_ROWS 5
#define NUM_COLS 5

typedef struct {
    int is_processed;
} Cell;

int main() {
    int shmid;
    Cell *garden;

    if ((shmid = shmget(SHM_KEY, NUM_ROWS * NUM_COLS * sizeof(Cell), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((garden = (Cell *)shmat(shmid, NULL, 0)) == (Cell *) -1) {
        perror("shmat");
        exit(1);
    }

    for (int i = 0; i < NUM_ROWS; ++i) {
        for (int j = 0; j < NUM_COLS; ++j) {
            garden[i * NUM_COLS + j].is_processed = 0;
        }
    }

    if (shmdt(garden) == -1) {
        perror("shmdt");
        exit(1);
    }

    printf("Инициализация сада завершена.\n");

    return 0;
}
