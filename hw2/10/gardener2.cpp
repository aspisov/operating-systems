#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>

const char* SEM_NAME = "/garden_sem";
const char* PIPE_NAME = "/tmp/garden_pipe";

int main() {
    sem_t* sem = sem_open(SEM_NAME, 0);
    int pipe_fd = open(PIPE_NAME, O_RDONLY);

    if (sem == SEM_FAILED || pipe_fd == -1) {
        perror("Error initializing sem or pipe");
        return 1;
    }

    char message[100];
    while (true) {
        read(pipe_fd, message, sizeof(message));
        std::cout << "Gardener 2: Received message - " << message << std::endl;
        sem_wait(sem);
        std::cout << "Gardener 2 is now working based on the received message." << std::endl;
        sem_post(sem);
        sleep(1);
    }

    close(pipe_fd);
    sem_close(sem);
    return 0;
}
