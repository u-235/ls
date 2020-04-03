/* utf-8 */

/**
 * @file
 * @brief Тестовое задание ls -l
 * @details Для каждого файла должно распечатать:
 *  - тип файла и режим доступа файла
 *  - количество символических ссылок
 *  - имя владельца
 *  - имя группы
 *  - размер
 *  - дату модификации
 *  - имя
 *
 * Если имя владельца или имя группы не может быть определено, должно распечатать вместо него
 * числовой идентификатор владельца или группы.
 *
 * Сортировка по алфавиту.
 *
 * @date создан 29.03.2020
 * @author Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>  /* типы для stat()                      */
#include <sys/stat.h>   /* stat() => инфо о файле               */
#include <dirent.h>     /* opendir() readdir() closedir()       */
#include <unistd.h>     /* chdir()                              */

#include "info.h"
#include "list.h"

int
main(int argc, char *argv[])
{
        struct stat sb;
        struct list_info_t *list = list_new();
        struct format_t format ={
                .cols={0, 0, 0, 0, 0, 0, 0}
        };

        if (argc != 2) {
                fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
                exit(EXIT_FAILURE);
        }

        if (stat(argv[1], &sb) == -1) {
                perror("stat() in main()");
                exit(EXIT_FAILURE);
        }

        if (S_ISDIR(sb.st_mode)) {
                /* Неочевидный момент: если на вход stat() подавать только имя файла, то функция
                 * пытается найти этот файл в текущей директории. А путь из аргумента запуска
                 * может и не совпадать с текущей директорией, поэтому меняем ее. */
                if(chdir(argv[1]) != 0){
                        perror("change directory");
                        exit(EXIT_FAILURE);
                }

                DIR * dirp = opendir(".");
                if (dirp == NULL) {
                        perror("couldn't open '.'");
                        exit(EXIT_FAILURE);
                }

                struct dirent *dp;
                while ((dp = readdir(dirp)) != NULL) {
                        /* В список элементов директории входят "лишних" два элемента - это
                         * читаемая директория и родительская.*/
                        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
                                list_add(list, info_new(dp->d_name));
                        }
                }

                closedir(dirp);
        } else {
                list_add(list, info_new(argv[1]));
        }

        list_sort(list);
        list_calc_columns(list, &format);
        list_print(list, &format);
        list_delete(list);

        return EXIT_SUCCESS;
}
