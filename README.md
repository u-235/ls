## ls на коленке

Минимальный аналог ls -l


### ТЗ

Для каждого файла должно распечатать:

- тип файла и режим доступа файла
- количество символических ссылок
- имя владельца или идентификатор
- имя группы или идентификатор
- размер
- дату модификации
- имя

Сортировка по алфавиту.

### Сборка

    cd build
    cmake ../
    cmake --build .

### Полезные ссылки

[opendir()](https://pubs.opengroup.org/onlinepubs/009695399/functions/opendir.html)
[readdir()](https://pubs.opengroup.org/onlinepubs/009695399/functions/readdir.html)
[closedir()](https://pubs.opengroup.org/onlinepubs/009695399/functions/closedir.html)
для чтения содержимого директории.
[stat()](https://pubs.opengroup.org/onlinepubs/009695399/functions/stat.html) позволяет получить информацию о файле.

https://www.opennet.ru/ -- тут можно почитать на русском.

