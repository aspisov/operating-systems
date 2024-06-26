# Отчет по домашнему заданию 11 на 10 баллов

## Описание

Этот проект включает реализацию клиент-серверного приложения, использующего протокол UDP для передачи сообщений. Приложение состоит из двух частей:
1. Широковещательная рассылка (Broadcast)
2. Многоадресная рассылка (Multicast)

### Широковещательная рассылка (Broadcast)
Сервер отправляет сообщения всем клиентам в локальной сети, используя широковещательный адрес `255.255.255.255`.

### Многоадресная рассылка (Multicast)
Сервер отправляет сообщения клиентам, подписанным на определённую группу многоадресной рассылки. В данном проекте используется группа `239.255.255.250`.

## Структура Проекта

- `broadcast_server.c` - Сервер для широковещательной рассылки
- `broadcast_client.c` - Клиент для широковещательной рассылки
- `multicast_server.c` - Сервер для многоадресной рассылки
- `multicast_client.c` - Клиент для многоадресной рассылки

## Как собрать

Для компиляции исходных файлов используйте следующий набор команд:

```sh
gcc broadcast_server.c -o broadcast_server
gcc broadcast_client.c -o broadcast_client
gcc multicast_server.c -o multicast_server
gcc multicast_client.c -o multicast_client
```

## Как запустить

### Широковещательная рассылка

1. Запустите сервер на одном из компьютеров в локальной сети:
    ```sh
    ./broadcast_server
    ```
2. Запустите клиент на других компьютерах в той же сети:
    ```sh
    ./broadcast_client
    ```

Теперь вы можете вводить сообщения на сервере, и они будут отображаться у всех запущенных клиентов.

### Многоадресная рассылка

1. Запустите сервер на одном из компьютеров в локальной сети:
    ```sh
    ./multicast_server
    ```
2. Запустите клиент на других компьютерах в той же сети:
    ```sh
    ./multicast_client
    ```

Сообщения, вводимые на сервере, будут получены всеми клиентами, подписанными на многоадресную группу.