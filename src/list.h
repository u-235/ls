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

#ifndef LIST_H_
#define LIST_H_

#include "info.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Структура списка описаний файлов. */
struct list_info_t {
        size_t capacity;            /* Вместимость массива items. */
        size_t length;              /* Количество указателей. */
        struct info_file_t **items; /* Указатель на массив указателей на структуры. */
        size_t cols[COLUMNS];       /* Массив значений ширины колонок. */
};

/**
 * Создание списка.
 * @return Указатель на list_info_t.
 */
extern struct list_info_t *
list_new();

/**
 * Удаление списка и всех его элементов.
 */
extern void
list_delete(
        struct list_info_t *list);

/**
 * Добавление описания файла.
 */
extern void
list_add(
        struct list_info_t *list,
        struct info_file_t *info);

/**
 * Сортировка списка.
 */
extern void
list_sort(
        struct list_info_t *list);

/**
 * Печать списка.
 */
extern void
list_print(
        struct list_info_t *list);


#ifdef __cplusplus
}
#endif

#endif /* LIST_H_ */
