# Отчет по домашнему заданию 10 на 10 баллов

#### Обзор проекта
Разработанное приложение представляет собой клиент-серверную систему на языке программирования C, использующую протокол TCP/IP для обмена сообщениями между двумя клиентами через сервер. Клиент №1 отправляет сообщения на сервер, который перенаправляет их клиенту №2. Передача сообщений продолжается до тех пор, пока не будет отправлено специальное сообщение "The End", после чего все компоненты системы завершают работу.

#### Состав системы
Система состоит из трех основных компонентов:
1. **Сервер (`server.c`)** - принимает сообщения от клиента №1 и перенаправляет их клиенту №2.
2. **Клиент №1 (`client1.c`)** - отправляет сообщения серверу.
3. **Клиент №2 (`client2.c`)** - принимает сообщения от сервера.

#### Компиляция программ
Программы компилируются с использованием компилятора GCC. Для каждого компонента системы необходимо выполнить следующие команды в терминале:

```bash
gcc server.c -o server
gcc client1.c -o client1
gcc client2.c -o client2
```

#### Инструкция по тестированию
1. **Запуск сервера:**
   Откройте терминал и запустите сервер, указав порт, на котором он будет слушать входящие соединения:
   ```bash
   ./server
   ```
2. **Запуск клиента №1:**
   Откройте новое окно терминала и запустите клиента №1, указав IP-адрес и порт сервера:
   ```bash
   ./client1 127.0.0.1 8080
   ```
3. **Запуск клиента №2:**
   Откройте еще одно окно терминала и запустите клиента №2, также указав IP-адрес и порт сервера:
   ```bash
   ./client2 127.0.0.1 8080
   ```
4. **Тестирование передачи сообщений:**
   Введите сообщения в терминале клиента №1. Убедитесь, что сообщения корректно отображаются в терминале клиента №2. После ввода "The End" все компоненты системы должны корректно завершить работу.