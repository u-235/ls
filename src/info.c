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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <grp.h>        /* для получения имени группы           */
#include <pwd.h>        /* для получения имени пользователя     */

#include "info.h"

/*
 * Заполняет поля структуры info для файла name. Так же обновляет позиции колонок pos.
 */
static void
info_fill(
        struct info_file_t *info,
        size_t cols[],
        const char *path);

static void
info_fill_mode(
        struct info_file_t *info,
        size_t cols[]);

static void
info_fill_link(
        struct info_file_t *info,
        size_t cols[]);

static void
info_fill_user(
        struct info_file_t *info,
        size_t cols[]);

static void
info_fill_group(
        struct info_file_t *info,
        size_t cols[]);

static void
info_fill_size(
        struct info_file_t *info,
        size_t cols[]);

static void
info_fill_time(
        struct info_file_t *info,
        size_t cols[]);

static void
info_fill_name(
        struct info_file_t *info,
        size_t cols[],
        const char *name);

struct info_file_t *
info_new(
        const char *path,
        size_t cols[])
{
        assert(path != NULL);

        struct info_file_t *info = calloc(1, sizeof(struct info_file_t));
        assert(info != NULL);
        info_fill(info, cols, path);
        return info;
}

void
info_delete(
        struct info_file_t *info)
{
        assert(info != NULL);

        for (int i = 0; i < COLUMNS; i++) {
                free(info->str[i]);
        }
}

int
info_compare(
        const struct info_file_t *l,
        const struct info_file_t *r)
{
        assert(l != NULL);
        assert(r != NULL);

        return strcmp(l->str[COL_NAME], r->str[COL_NAME]);
}

void
info_print(
        const struct info_file_t *info,
        size_t cols[])
{
        assert(info != NULL);
        assert(cols != NULL);

        size_t printed = 0;
        size_t tab = 0;

        for (int i = 0; i < COLUMNS; i++) {
                while (tab > printed) {
                        putchar(' ');
                        printed++;
                }

                /* Красивое выравнивание по правому краю для счетчика ссылок
                 * и размера файла.
                 */
                if (i == COL_LINK || i == COL_SIZE) {
                        size_t sz = cols[i] - strlen(info->str[i]);
                        while (tab + sz > printed) {
                                putchar(' ');
                                printed++;
                        }
                }

                tab = tab + cols[i] + 1;

                size_t num;
                printf("%s%n", info->str[i], (int *) &num);
                printed += num;
        }

        printf("\n");
}

void
info_fill(
        struct info_file_t *info,
        size_t cols[],
        const char *path)
{
        assert(info != NULL);
        assert(cols != NULL);
        assert(path != NULL);

        if (stat(path, &info->bin) == -1) {
                perror("stat() in info_fill()");
                exit(EXIT_FAILURE);
        }

        /* Поскольку имя файла может попасть из аргументов запуска, оно может содержать
         * элементы пути к файлу. Это оставляет только имя. */
        const char *name = strrchr(path, '/');
        if (name == NULL) {
                name = path;
        } else {
                name++;
        }

        info_fill_mode(info, cols);
        info_fill_link(info, cols);
        info_fill_user(info, cols);
        info_fill_group(info, cols);
        info_fill_size(info, cols);
        info_fill_time(info, cols);
        info_fill_name(info, cols, name);
}

void
info_fill_mode(
        struct info_file_t *info,
        size_t cols[])
{
        mode_t mode = info->bin.st_mode;

#define MODE_STAMP "?rwxrwxrwx"
        size_t sz = sizeof(MODE_STAMP) - 1;
        char *ms = malloc(sz + 1);
        assert(ms != NULL);
        strcpy(ms, MODE_STAMP);

        switch (mode & S_IFMT) {
        case S_IFBLK:
                ms[0] = 'b';
                break;
        case S_IFCHR:
                ms[0] = 'c';
                break;
        case S_IFDIR:
                ms[0] = 'd';
                break;
        case S_IFIFO:
                ms[0] = 'p';
                break;
        case S_IFLNK:
                ms[0] = 'l';
                break;
        case S_IFREG:
                ms[0] = '-';
                break;
        case S_IFSOCK:
                ms[0] = 's';
                break;
        default:
                ;
        }

#if S_IRUSR != 0400 || S_IXOTH != 0001
#error Check S_I*USR S_I*GRP S_I*OTH constants
#endif

        mode_t mask = S_IRUSR;
        char *ptr = ms;
        ptr++;

        do {
                if (!(mode & mask)) {
                        *ptr = '-';
                }

                ptr++;
                mask = mask >> 1;
        } while (mask != S_IXOTH);

        info->str[COL_MODE] = ms;
        cols[COL_MODE] = sz;

#undef MODE_STAMP
}

void
info_fill_link(
        struct info_file_t *info,
        size_t cols[])
{
        char buf[256];
        size_t sz;

        sprintf(buf, "%d%n", info->bin.st_nlink, (int *) &sz);
        char *str = malloc(sz + 1);
        assert(str != NULL);
        strcpy(str, buf);
        info->str[COL_LINK] = str;
        cols[COL_LINK] = cols[COL_LINK] > sz ? cols[COL_LINK] : sz;
}

void
info_fill_user(
        struct info_file_t *info,
        size_t cols[])
{
        uid_t uid = info->bin.st_uid;
        char buf[256];
        size_t sz;

        char *src;
        struct passwd *pwd = getpwuid(uid);
        if (pwd != NULL) {
                sz = strlen(pwd->pw_name);
                src = pwd->pw_name;
        } else {
                sprintf(buf, "%d%n", uid, (int *) &sz);
                src = buf;
        }

        char *dst = malloc(sz + 1);
        assert(dst != NULL);
        strcpy(dst, src);
        info->str[COL_USER] = dst;
        cols[COL_USER] = cols[COL_USER] > sz ? cols[COL_USER] : sz;
}

void
info_fill_group(
        struct info_file_t *info,
        size_t cols[])
{
        gid_t gid = info->bin.st_gid;
        char buf[256];
        size_t sz;

        char *src;
        struct group * gr_info = getgrgid(gid);
        if (gr_info != NULL) {
                sz = strlen(gr_info->gr_name);
                src = gr_info->gr_name;
        } else {
                sprintf(buf, "%d%n", gid, (int *) &sz);
                src = buf;
        }

        char *dst = malloc(sz + 1);
        assert(dst != NULL);
        strcpy(dst, src);
        info->str[COL_GROUP] = dst;
        cols[COL_GROUP] = cols[COL_GROUP] > sz ? cols[COL_GROUP] : sz;
}

void
info_fill_size(
        struct info_file_t *info,
        size_t cols[])
{
        char buf[256];
        size_t sz;
        sprintf(buf, "%ld%n", info->bin.st_size, (int *) &sz);
        char *dst = malloc(sz + 1);
        assert(dst != NULL);
        strcpy(dst, buf);
        info->str[COL_SIZE] = dst;
        cols[COL_SIZE] = cols[COL_SIZE] > sz ? cols[COL_SIZE] : sz;
}

void
info_fill_time(
        struct info_file_t *info,
        size_t cols[])
{
        char buf[256];
        size_t sz = strftime(buf, 256, "%X %Ex", gmtime((const time_t *) &info->bin.st_mtim));
        char *dst = malloc(sz + 1);
        assert(dst != NULL);
        strcpy(dst, buf);
        info->str[COL_DATE] = dst;
        cols[COL_DATE] = cols[COL_DATE] > sz ? cols[COL_DATE] : sz;
}

void
info_fill_name(
        struct info_file_t *info,
        size_t cols[] __attribute__((unused)),
        const char *name)
{
        char *dst = malloc(strlen(name) + 1);
        assert(dst != NULL);
        strcpy(dst, name);
        info->str[COL_NAME] = dst;
}

