#!/bin/bash
# Этот скрипт - простая игра "Угадай число".

# Функция, генерирующая случайное число от 1 до 100
generate_number() {
  echo $(( (RANDOM % 100) + 1 ))
}

# Функция для предложения пользователю угадать число
prompt_guess() {
  echo "Угадайте число от 1 до 100"
  read guess
}

# Основной игровой цикл
play_game() {
  local target=$(generate_number)
  local guess=-1

  # Продолжаем предлагать пользователю угадать число, пока не будет угадано правильное
  while [[ $guess -ne $target ]]; do
    prompt_guess

    # Проверяем, правильно ли угадано число
    if [[ $guess -eq $target ]]; then
      echo "Поздравляем! Вы угадали правильное число: $target"
      break
    elif [[ $guess -lt $target ]]; then
      echo "Больше!"
    else
      echo "Меньше!"
    fi
  done
}

# Начало игры
play_game