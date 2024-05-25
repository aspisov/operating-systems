#!/bin/bash

depth=0
filename="a"
linkname=""

# Создаем начальный регулярный файл "a"
touch $filename

# Запускаем процесс рекурсии
while true; do
  linkname="${filename}_$depth"
  ln -s $filename $linkname
  if ! cat $linkname >/dev/null 2>&1; then
    break
  fi
  filename=$linkname
  depth=$((depth + 1))
done

echo "Максимальная глубина рекурсии символьных ссылок: $depth"

# Очистка
for ((i=depth-1; i>=0; i--)); do
  linkname="a_$i"
  rm $linkname
done
rm a
