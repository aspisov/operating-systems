# Отчет по домашнему заданию №5

## Описание

Данный проект позволяет передавать целое десятичное число от одного процесса к другому с использованием пользовательских сигналов SIGUSR1 и SIGUSR2 в операционной системе Linux. Проект состоит из двух программ: передатчика и приемника.

Передатчик отправляет каждый бит числа, используя SIGUSR1 для бита 0 и SIGUSR2 для бита 1, при этом ожидает подтверждение приема каждого бита от приемника. Приемник принимает биты, собирает из них исходное число и выводит его.

## Требования

Для сборки и запуска программ необходим компилятор C (например, gcc) и операционная система Linux.

## Компиляция

Для компиляции передатчика и приемника выполните следующие команды в терминале:

```bash
gcc sender.c -o sender
gcc receiver.c -o receiver
```

Это создаст два исполняемых файла: `sender` и `receiver`.

## Использование

1. Откройте два терминала. В одном терминале запустите приемник, в другом — передатчик.
2. В терминале приемника запустите программу `./receiver`. Программа выведет свой PID и попросит ввести PID передатчика.
3. В терминале передатчика запустите программу `./sender`. Программа также выведет свой PID и запросит PID приемника.
4. Введите PID приемника в программу передатчика и PID передатчика в программу приемника, когда будет запрошено.
5. Введите целое десятичное число для передачи в программу передатчика.
6. Приемник получит число и выведет его после завершения передачи.

## Примечания

- Передача данных осуществляется асинхронно и требует подтверждения приема каждого бита, что обеспечивает надежность передачи.
- Подтверждение приема бита реализуется отправкой сигнала обратно от приемника к передатчику.
- В данной реализации не предусмотрена специальная логика для сигнализирования о завершении передачи, поэтому рекомендуется доработать этот момент для обеспечения корректного завершения сессии передачи данных.