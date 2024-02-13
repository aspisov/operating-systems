#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Использование: " << argv[0] << " <источник> <назначение>" << std::endl;
        return 1;
    }

    const char* source_file = argv[1];
    const char* dest_file = argv[2];
    const int buf_size = 1024; // Размер буфера можно изменить в соответствии с требованиями
    char buffer[buf_size];

    // Открытие исходного файла для чтения
    int source_fd = open(source_file, O_RDONLY);
    if (source_fd == -1) {
        perror("Ошибка при открытии исходного файла");
        return 2;
    }

    // Получение режима доступа исходного файла
    struct stat stat_buf;
    if (fstat(source_fd, &stat_buf) == -1) {
        perror("Ошибка при получении информации о файле");
        close(source_fd);
        return 3;
    }

    // Открытие целевого файла для записи с режимом доступа исходного файла
    int dest_fd = open(dest_file, O_WRONLY | O_CREAT | O_TRUNC, stat_buf.st_mode);
    if (dest_fd == -1) {
        perror("Ошибка при открытии целевого файла");
        close(source_fd);
        return 4;
    }

    ssize_t bytes_read, bytes_written;
    while ((bytes_read = read(source_fd, buffer, buf_size)) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written == -1) {
            perror("Ошибка при записи в целевой файл");
            close(source_fd);
            close(dest_fd);
            return 5;
        }
    }

    if (bytes_read == -1) {
        perror("Ошибка при чтении из исходного файла");
        close(source_fd);
        close(dest_fd);
        return 6;
    }

    // Установка режима доступа исходного файла для целевого файла
    if (chmod(dest_file, stat_buf.st_mode) == -1) {
        perror("Ошибка при установке режима доступа целевого файла");
        close(source_fd);
        close(dest_fd);
        return 7;
    }

    close(source_fd);
    close(dest_fd);
    std::cout << "Файл успешно скопирован." << std::endl;
    return 0;
}
