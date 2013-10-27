/***************************************************************************
 *   Copyright (C) 2013~2013 by Yichao Yu                                  *
 *   yyc1992@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.              *
 ***************************************************************************/

#ifndef _QTC_UTILS_UTILS_H_
#define _QTC_UTILS_UTILS_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <math.h>
#ifndef __cplucplus
#  include <stdbool.h>
#endif

typedef struct {
    int len;
    int width;
    int height;
    int depth;
    const unsigned char *data;
} QtcPixmap;

#define qtcContainerOf(ptr, type, member)               \
    ((type*)(((void*)(ptr)) - offsetof(type, member)))

#if (defined(__GNUC__) && (__GNUC__ > 2))
#  define qtcExpect(exp, var) __builtin_expect(exp, var)
#else
#  define qtcExpect(exp, var) (exp)
#endif

#define qtcLikely(x) qtcExpect(!!(x), 1)
#define qtcUnlikely(x) qtcExpect(!!(x), 0)

#define QTC_EXPORT __attribute__((visibility("default")))

#ifdef __cplusplus
#  define QTC_BEGIN_DECLS extern "C" {
#  define QTC_END_DECLS }
#else
#  define QTC_BEGIN_DECLS
#  define QTC_END_DECLS
#endif

#define QTC_ALWAYS_INLINE __attribute__((always_inline))
#define QTC_UNUSED(x) ((void)(x))

#define _QTC_USE_1(x, y) x
#define _QTC_USE_2(x, y) y
#define __QTC_USE_12(dummy, v, res, ...) res
#define _QTC_USE_12(v...) __QTC_USE_12(0, ##v, _QTC_USE_1, _QTC_USE_2)
#define QTC_SWITCH(v, t, f) _QTC_USE_12(v)(t, f)
#define QTC_DEFAULT(v, def) QTC_SWITCH(v, v, def)
#define QTC_SWITCH_(v, f) QTC_SWITCH(v, f, _##f)

QTC_ALWAYS_INLINE static inline void*
qtcAlloc0(size_t size)
{
    void *p = malloc(size);
    memset(p, 0, size);
    return p;
}

QTC_ALWAYS_INLINE static inline void
qtcFree(void *p)
{
    if (p) {
        free(p);
    }
}

#define qtcNewSize(type, size) ((type*)qtcAlloc0(size))
#define qtcNew(type) qtcNewSize(type, sizeof(type))
#define qtcNewN(type, n) qtcNewSize(type, sizeof(type) * n)

#ifdef __cplusplus

#define __QTC_USE_DECLVAL
#if defined __cplusplus && defined __GNUC__ && !defined __clang__
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 8) ||            \
    (__GNUC__ == 4 && __GNUC_MINOR__ == 8 && __GNUC_PATCHLEVEL__ < 1)
#undef __QTC_USE_DECLVAL
#endif
#endif

#ifdef __QTC_USE_DECLVAL
#include <utility>
#define _QTC_COMP_TYPE(T1, T2)                                  \
    decltype(0 ? std::declval<T1>() : std::declval<T2>())
#else
#define _QTC_COMP_TYPE(T1, T2)                  \
    decltype(0 ? T1() : T2())
#endif
#endif

#ifdef __cplusplus
template <class T>
const T&
const_(const T &t)
{
    return t;
}
#endif

#define QTC_BUFF_TYPE(type)                     \
    struct {                                    \
        union {                                 \
            type *p;                            \
            void *_p;                           \
        };                                      \
        size_t l;                               \
        type *const static_p;                   \
        const size_t static_l;                  \
    }

#define QTC_DEF_LOCAL_BUFF(type, name, stack_size, size)                \
    type __##qtc_local_buff##name[stack_size];                          \
    QTC_BUFF_TYPE(type) name = {                                        \
        {__##qtc_local_buff##name},                                     \
        sizeof(__##qtc_local_buff##name) / sizeof(type),                \
        __##qtc_local_buff##name,                                       \
        sizeof(__##qtc_local_buff##name) / sizeof(type)                 \
    };                                                                  \
    QTC_RESIZE_LOCAL_BUFF(name, size)

#define QTC_RESIZE_LOCAL_BUFF(name, size) do {                          \
        size_t __new_size = (size);                                     \
        if (__new_size <= (name).l || __new_size <= (name).static_l)    \
            break;                                                      \
        (name).l = __new_size;                                          \
        size_t __alloc_size = sizeof(*(name).p) * __new_size;           \
        if ((name).p == (name).static_p) {                              \
            (name)._p = malloc(__alloc_size);                           \
        } else {                                                        \
            (name)._p = realloc((name)._p, __alloc_size);               \
        }                                                               \
    } while (0)

#define QTC_FREE_LOCAL_BUFF(name) do {          \
        if ((name).p != (name).static_p) {      \
            free((name)._p);                    \
        }                                       \
    } while (0)

QTC_BEGIN_DECLS

void *qtcBSearch(const void *key, const void *base, size_t nmemb, size_t size,
                 int (*compar)(const void*, const void*));

typedef struct {
    const char *key;
    int val;
} QtcStrMapItem;

typedef struct {
    unsigned num;
    QtcStrMapItem *items;
    bool auto_val;
    bool inited;
    bool case_sensitive;
} QtcStrMap;

void qtcStrMapInit(QtcStrMap *map);
#define QTC_DECL_STR_MAP(map, case_sense, items...)      \
    static QtcStrMapItem __##map##_items[] = {items};    \
    static QtcStrMap map = {                             \
        sizeof(__##map##_items) / sizeof(QtcStrMapItem), \
        __##map##_items,                                 \
        false,                                           \
        false,                                           \
        case_sense,                                      \
    };                                                   \
    if (!map.inited) {                                   \
        qtcStrMapInit(&map);                             \
    }

static inline void
qtcStrMapInitKeys(QtcStrMap *map, const char **keys)
{
    if (map->inited) {
        return;
    }
    for (unsigned i = 0;i < map->num;i++) {
        map->items[i].key = keys[i];
    }
    qtcStrMapInit(map);
}

#define QTC_DECL_STR_MAP_AUTO(map, case_sense, keys...)           \
    static const char *__##map##_keys[] = {keys};                 \
    static QtcStrMapItem __##map##_items[sizeof(__##map##_keys) / \
                                         sizeof(const char*)];    \
    static QtcStrMap map = {                                      \
        sizeof(__##map##_keys) / sizeof(const char*),             \
        __##map##_items,                                          \
        true,                                                     \
        false,                                                    \
        case_sense,                                               \
    };                                                            \
    qtcStrMapInitKeys(&map, __##map##_keys);

int qtcStrMapSearch(const QtcStrMap *map, const char *key, int def);
#define _qtcStrMapSearch(map, key, def, ...)    \
    (qtcStrMapSearch)(map, key, def)
#define qtcStrMapSearch(map, key, def...)       \
    _qtcStrMapSearch(map, key, ##def, 0)

QTC_END_DECLS

#endif
