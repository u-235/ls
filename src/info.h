/* utf-8 */

/**
 * @file
 * @brief
 * @details
 *
 * @date создан 31.03.2020
 * @author Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef INFO_H_
#define INFO_H_

#include <sys/types.h>  /* типы для stat()        */
#include <sys/stat.h>   /* stat() => инфо о файле */

#include "format.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Каждому печатаемому полю структуры stat соответствует строка str.
 * Структуру можно создать функцией info_new и удалить функцией info_delete.
 */
struct info_file_t {
        struct stat bin;
        char *str[COLUMNS];
};

/**
 * Создает структуру info_file_t
 * @param path Путь и имя или только имя файла.
 * @param cols Массив значений ширины колонок. Если строка больше, то соответствующее значение
 *              увеличивается.
 * @return Указатель на структуру info_file_t
 */
extern struct info_file_t *
info_new(
        const char *path);

/**
 * Удаление структуры.
 */
extern void
info_delete(
        struct info_file_t *info);

/**
 * Сравнение структур. В настоящее время это алфавитное сравнение.
 * @return Если l < r, то возвращается отрицательное значение; если l > r, то возвращается
 *      положительное значение, иначе возвращается 0.
 */
extern int
info_compare(
        const struct info_file_t *l,
        const struct info_file_t *r);

/**
 *
 */
extern void
info_calc_colunmss(
        const struct info_file_t *info,
        size_t cols[]);

/**
 * Печать строк структуры info. Ширина колонок подгоняется в соответствии со значениями cols.
 */
extern void
info_print(
        const struct info_file_t *info,
        const size_t cols[]);


#ifdef __cplusplus
}
#endif

#endif /* INFO_H_ */
