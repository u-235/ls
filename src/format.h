/* utf-8 */

/**
 * @file
 * @brief
 * @details
 *
 * @date создан 03.04.2020
 * @author Nick Egorrov
 */

#ifndef FORMAT_H_
#define FORMAT_H_

#include <stddef.h> /* size_t */

#ifdef __cplusplus
extern "C" {
#endif

/* Константы для обозначения номера колонки и общего числа колонок печати. */
#define COL_MODE        0
#define COL_LINK        1
#define COL_USER        2
#define COL_GROUP       3
#define COL_SIZE        4
#define COL_DATE        5
#define COL_NAME        6
#define COLUMNS         7

/* Структура для опций форматирования печати. */
struct format_t {
        size_t cols[COLUMNS];       /* Массив значений ширины колонок. */
};


#ifdef __cplusplus
}
#endif

#endif /* FORMAT_H_ */
