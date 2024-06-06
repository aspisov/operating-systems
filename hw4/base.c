#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define M 10  // Number of rows
#define N 10  // Number of columns
#define PERCENT_OBSTACLES_MIN 10
#define PERCENT_OBSTACLES_MAX 30

typedef struct {
    int x;
    int y;
} Position;

int garden[M][N];
pthread_mutex_t garden_mutex = PTHREAD_MUTEX_INITIALIZER;

void initialize_garden() {
    srand(time(0));
    int obstacles = PERCENT_OBSTACLES_MIN + rand() % (PERCENT_OBSTACLES_MAX - PERCENT_OBSTACLES_MIN + 1);
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            garden[i][j] = (rand() % 100 < obstacles) ? -1 : 0;
        }
    }
}

void print_garden() {
    system("clear");
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            if (garden[i][j] == -1) {
                printf("XX ");
            } else if (garden[i][j] == 1) {
                printf("G1 ");
            } else if (garden[i][j] == 2) {
                printf("G2 ");
            } else {
                printf("__ ");
            }
        }
        printf("\n");
    }
    usleep(500000);
}

void* gardener1(void* arg) {
    Position* pos = (Position*)arg;
    int direction = 1;  // 1 for moving right, -1 for moving left
    while (pos->x < M) {
        pthread_mutex_lock(&garden_mutex);
        if (garden[pos->x][pos->y] == 0) {
            garden[pos->x][pos->y] = 1;
        }
        pthread_mutex_unlock(&garden_mutex);
        print_garden();

        if (direction == 1 && pos->y + 1 < N) {
            pos->y++;
        } else if (direction == -1 && pos->y - 1 >= 0) {
            pos->y--;
        } else {
            pos->x++;
            direction *= -1;
        }

        usleep(100000);
    }
    return NULL;
}

void* gardener2(void* arg) {
    Position* pos = (Position*)arg;
    int direction = -1;  // -1 for moving up, 1 for moving down
    while (pos->x >= 0) {
        pthread_mutex_lock(&garden_mutex);
        if (garden[pos->x][pos->y] == 0) {
            garden[pos->x][pos->y] = 2;
        }
        pthread_mutex_unlock(&garden_mutex);
        print_garden();

        if (direction == -1 && pos->x - 1 >= 0) {
            pos->x--;
        } else if (direction == 1 && pos->x + 1 < M) {
            pos->x++;
        } else {
            pos->y--;
            direction *= -1;
        }

        usleep(100000);
    }
    return NULL;
}

int main() {
    initialize_garden();

    Position gardener1_pos = {0, 0};
    Position gardener2_pos = {M - 1, N - 1};

    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, gardener1, &gardener1_pos);
    pthread_create(&thread2, NULL, gardener2, &gardener2_pos);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}
