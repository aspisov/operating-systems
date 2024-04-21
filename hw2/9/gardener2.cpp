#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <semaphore.h>
#include <unistd.h>

int main() {
    mqd_t mq = mq_open("/garden_queue", O_CREAT | O_RDONLY, 0666, NULL);
    if (mq == -1) {
        perror("mq_open");
        exit(1);
    }

    sem_t *sem = sem_open("/garden_sem", 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    char message[1024];
    while (true) {
        if (mq_receive(mq, message, 1024, NULL) == -1) {
            perror("mq_receive");
            exit(1);
        }
        std::cout << "Message received: " << message << std::endl;
        sem_wait(sem);
        std::cout << "Gardener 2 is working based on the message.\n";
        sleep(1);  // Имитация работы
        sem_post(sem);
        sleep(1);  // Перерыв
    }

    mq_close(mq);
    sem_close(sem);
    return 0;
}
