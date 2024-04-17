//
// Created by Dmitry on 4/17/24.
//
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>
#include <cstring>

#define PIPE_FD_READ 0
#define PIPE_FD_WRITE 1

sem_t *sem_parent, *sem_child;

void cleanup(int signum) {
    sem_close(sem_parent);
    sem_close(sem_child);
    sem_unlink("/sem_parent");
    sem_unlink("/sem_child");
    std::cout << "Semaphores unlinked and closed." << std::endl;
    exit(0);
}

int main() {
    int pipe_fd[2];
    pid_t pid;
    const int msg_count = 10;
    char buf[1024];

    // Сигнал для остановки
    signal(SIGINT, cleanup);

    // Создание семафоров
    sem_parent = sem_open("/sem_parent", O_CREAT, 0644, 1);
    sem_child = sem_open("/sem_child", O_CREAT, 0644, 0);

    // Создание канала
    if (pipe(pipe_fd) == -1) {
        std::cerr << "Failed to create pipe." << std::endl;
        return 1;
    }

    // Создание дочернего процесса
    pid = fork();
    if (pid == -1) {
        std::cerr << "Failed to fork." << std::endl;
        return 1;
    }

    if (pid == 0) {  // Дочерний процесс
        close(pipe_fd[PIPE_FD_WRITE]);  // Закрыть неиспользуемый конец
        for (int i = 0; i < msg_count; ++i) {
            sem_wait(sem_child);
            read(pipe_fd[PIPE_FD_READ], buf, sizeof(buf));
            std::cout << "Child received: " << buf << std::endl;
            snprintf(buf, sizeof(buf), "Message from child %d", i + 1);
            sem_post(sem_parent);
        }
        close(pipe_fd[PIPE_FD_READ]);
    } else {  // Родительский процесс
        close(pipe_fd[PIPE_FD_READ]);  // Закрыть неиспользуемый конец
        for (int i = 0; i < msg_count; ++i) {
            snprintf(buf, sizeof(buf), "Message from parent %d", i + 1);
            sem_wait(sem_parent);
            write(pipe_fd[PIPE_FD_WRITE], buf, strlen(buf) + 1);
            sem_post(sem_child);
        }
        close(pipe_fd[PIPE_FD_WRITE]);
        wait(NULL);  // Ожидание завершения дочернего процесса
    }

    cleanup(0);
    return 0;
}
