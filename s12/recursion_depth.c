#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int depth = 0;
    int fd;
    char filename[256] = "a";
    char linkname[256];

    // Создаем начальный регулярный файл "a"
    fd = open(filename, O_CREAT | O_WRONLY, 0644);
    if (fd == -1) {
        perror("Не удалось создать начальный файл");
        return 1;
    }
    close(fd);

    // Запускаем процесс рекурсии
    while (1) {
        snprintf(linkname, sizeof(linkname), "%s_%d", filename, depth);
        if (symlink(filename, linkname) == -1) {
            perror("Не удалось создать символьную ссылку");
            break;
        }
        fd = open(linkname, O_RDONLY);
        if (fd == -1) {
            perror("Не удалось открыть символьную ссылку");
            break;
        }
        close(fd);
        snprintf(filename, sizeof(filename), "%s", linkname);
        depth++;
    }

    printf("Максимальная глубина рекурсии символьных ссылок: %d\n", depth);
    
    // Очистка
    for (int i = depth - 1; i >= 0; i--) {
        snprintf(linkname, sizeof(linkname), "a_%d", i);
        unlink(linkname);
    }
    unlink("a");
    
    return 0;
}
