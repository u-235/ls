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
        const char *path);

static void
info_fill_mode(
        struct info_file_t *info);

static void
info_fill_link(
        struct info_file_t *info);

static void
info_fill_user(
        struct info_file_t *info);

static void
info_fill_group(
        struct info_file_t *info);

static void
info_fill_size(
        struct info_file_t *info);

static void
info_fill_time(
        struct info_file_t *info);

static void
info_fill_name(
        struct info_file_t *info,
        const char *name);

struct info_file_t *
info_new(
        const char *path)
{
        assert(path != NULL);

        struct info_file_t *info = calloc(1, sizeof(struct info_file_t));
        assert(info != NULL);
        info_fill(info, path);
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
info_calc_colunmss(
        const struct info_file_t *info,
        size_t cols[])
{
        assert(info != NULL);
        assert(cols != NULL);

        size_t sz = 0;

        for (int i = 0; i < COLUMNS; i++) {
                sz = strlen(info->str[i]);

                cols[i] = cols[i] > sz ? cols[i] : sz;
        }
}

void
info_print(
        const struct info_file_t *info,
        const size_t cols[])
{
        assert(info != NULL);
        assert(cols != NULL);

        size_t printed = 0;
        size_t tab = 0;

        for (int i = 0; i < COLUMNS; i++) {
                do {
                        putchar(' ');
                        printed++;
                } while (tab > printed);

                /* Красивое выравнивание по правому краю для счетчика ссылок
                 * и размера файла.
                 */
                if (i == COL_LINK || i == COL_SIZE) {
                        size_t sz = strlen(info->str[i]);
                        while (tab + cols[i] >= printed + sz) {
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
        const char *path)
{
        assert(info != NULL);
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

        info_fill_mode(info);
        info_fill_link(info);
        info_fill_user(info);
        info_fill_group(info);
        info_fill_size(info);
        info_fill_time(info);
        info_fill_name(info, name);
}

void
info_fill_mode(
        struct info_file_t *info)
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

#undef MODE_STAMP
}

void
info_fill_link(
        struct info_file_t *info)
{
        char buf[256];
        size_t sz;

        sprintf(buf, "%d%n", info->bin.st_nlink, (int *) &sz);
        char *str = malloc(sz + 1);
        assert(str != NULL);
        strcpy(str, buf);
        info->str[COL_LINK] = str;
}

void
info_fill_user(
        struct info_file_t *info)
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
}

void
info_fill_group(
        struct info_file_t *info)
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
}

void
info_fill_size(
        struct info_file_t *info)
{
        char buf[256];
        size_t sz;
        sprintf(buf, "%ld%n", info->bin.st_size, (int *) &sz);
        char *dst = malloc(sz + 1);
        assert(dst != NULL);
        strcpy(dst, buf);
        info->str[COL_SIZE] = dst;
}

void
info_fill_time(
        struct info_file_t *info)
{
        char buf[256];
        size_t sz = strftime(buf, 256, "%X %Ex", gmtime((const time_t *) &info->bin.st_mtim));
        char *dst = malloc(sz + 1);
        assert(dst != NULL);
        strcpy(dst, buf);
        info->str[COL_DATE] = dst;
}

void
info_fill_name(
        struct info_file_t *info,
        const char *name)
{
        char *dst = malloc(strlen(name) + 1);
        assert(dst != NULL);
        strcpy(dst, name);
        info->str[COL_NAME] = dst;
}
