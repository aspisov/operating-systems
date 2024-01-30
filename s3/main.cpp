#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>

// Функция для вычисления числа Фибоначчи
unsigned long long fibonacci(unsigned n) {
    unsigned long long a = 0, b = 1, c;
    if (n == 0) return a;
    for (unsigned i = 2; i <= n; i++) {
        c = a + b;
        if (c < a) { // Проверка на переполнение
            std::cerr << "Переполнение при вычислении Фибоначчи" << std::endl;
            exit(EXIT_FAILURE);
        }
        a = b;
        b = c;
    }
    return b;
}

// Функция для вычисления факториала
unsigned long long factorial(unsigned n) {
    unsigned long long result = 1;
    for (unsigned i = 2; i <= n; i++) {
        result *= i;
        if (result < i) { // Проверка на переполнение
            std::cerr << "Переполнение при вычислении факториала" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return result;
}

// Функция для вывода информации о процессе
void printProcessInfo() {
    std::cout << "PID: " << getpid() << ", PPID: " << getppid() << std::endl;
}

// Функция для вывода содержимого текущего каталога
void printCurrentDirectory() {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(".")) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            std::cout << ent->d_name << std::endl;
        }
        closedir(dir);
    } else {
        perror("Не удается открыть текущий каталог");
    }
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <number>" << std::endl;
        return 1;
    }

    unsigned n = std::atoi(argv[1]);

    pid_t pid = fork();

    if (pid == -1) {
        std::cerr << "Fork failed" << std::endl;
        return 1;
    }

    if (pid > 0) {
        // Родительский процесс
        wait(nullptr);
        unsigned long long fibo = fibonacci(n);
        std::cout << "Фибоначчи(" << n << ") = " << fibo << std::endl;
        printProcessInfo();
        printCurrentDirectory();
    } else {
        // Дочерний процесс
        unsigned long long fact = factorial(n);
        std::cout << "Факториал(" << n << ") = " << fact << std::endl;
        printProcessInfo();
        exit(0);
    }

    return 0;
}
