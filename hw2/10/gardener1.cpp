#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>

const char* SEM_NAME = "/garden_sem";
const char* PIPE_NAME = "/tmp/garden_pipe";

int main() {
    sem_t* sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    mkfifo(PIPE_NAME, 0666);
    int pipe_fd = open(PIPE_NAME, O_WRONLY);

    if (sem == SEM_FAILED || pipe_fd == -1) {
        perror("Error initializing sem or pipe");
        return 1;
    }

    char message[100] = "Gardener 1 has finished working on the plot";
    while (true) {
        sem_wait(sem);
        write(pipe_fd, message, sizeof(message));
        std::cout << "Gardener 1: Sent message." << std::endl;
        sem_post(sem);
        sleep(1);
    }

    close(pipe_fd);
    unlink(PIPE_NAME);
    sem_close(sem);
    sem_unlink(SEM_NAME);
    return 0;
}
