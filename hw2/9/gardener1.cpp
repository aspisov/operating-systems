#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <semaphore.h>
#include <unistd.h>

int main() {
    mqd_t mq;
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 1024;
    attr.mq_curmsgs = 0;

    mq = mq_open("/garden_queue", O_CREAT | O_WRONLY, 0666, &attr);
    if (mq == -1) {
        perror("mq_open");
        exit(1);
    }

    sem_t *sem = sem_open("/garden_sem", O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    char message[1024] = "Gardener 1 finished work";
    while (true) {
        sem_wait(sem);
        std::cout << "Gardener 1 is working.\n";
        sleep(1);  // Имитация работы
        if (mq_send(mq, message, sizeof(message), 0) == -1) {
            perror("mq_send");
            exit(1);
        }
        sem_post(sem);
        sleep(1);  // Перерыв между работами
    }

    mq_close(mq);
    sem_close(sem);
    sem_unlink("/garden_sem");
    mq_unlink("/garden_queue");
    return 0;
}
