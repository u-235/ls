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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"


struct list_info_t *
list_new()
{
        struct list_info_t * list = calloc(1, sizeof(struct list_info_t));
        assert(list != NULL);
        return list;
}

void
list_delete(
        struct list_info_t *list)
{
        assert(list != NULL);

        for (size_t c = 0; c < list->length; c++) {
                info_delete(list->items[c]);
        }

        free(list->items);
        free(list);
}

void
list_add(
        struct list_info_t *list,
        struct info_file_t *info)
{
        assert(list != NULL);
        assert(info != NULL);

        /* При необходимости увеличиваем память под указатели на структуры file_info. */
        if (list->capacity <= list->length) {
                struct info_file_t **old = list->items;
                list->capacity += 8;
                list->items = calloc(list->capacity, sizeof(struct info_file_t *));
                assert(list->items != NULL);
                memcpy(list->items, old, sizeof(struct info_file_t *) * (list->capacity - 8));
                free(old);
        }

        list->items[list->length++] = info;
}

void
list_sort(
        struct list_info_t *list)
{
        assert(list != NULL);

        size_t head = 0;

        while (head < list->length - 1) {
                struct info_file_t *first = list->items[head];
                struct info_file_t *second = list->items[head + 1];

                if (info_compare(first, second) <= 0) {
                        head++;
                        continue;
                }

                list->items[head] = second;
                list->items[head + 1] = first;

                if (head > 0) {
                        head--;
                } else {
                        head++;
                }
        }
}

void
list_print(
        struct list_info_t *list)
{
        assert(list != NULL);

        for (size_t c = 0; c < list->length; c++) {
                info_print(list->items[c], list->cols);
        }
}


