/***************************************************************************
 *   Copyright (C) 2012~2013 by Yichao Yu                                  *
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

#include "utils.h"
#include "atomic.h"

#ifdef __QTC_ATOMIC_USE_SYNC_FETCH
/**
 * Also define lib function when there is builtin function for
 * atomic operation in case the function address is needed or the builtin
 * is not available when compiling other modules.
 **/
#define QTC_DEFINE_ATOMIC(name, op, type)               \
    QTC_EXPORT type                                     \
    (qtcAtomic##name)(volatile type *atomic, type val)  \
    {                                                   \
        return __qtcAtomic##name(atomic, val);          \
    }
#else
#include <pthread.h>
static pthread_mutex_t __qtc_atomic_lock = PTHREAD_MUTEX_INITIALIZER;
#define QTC_DEFINE_ATOMIC(name, op, type)               \
    QTC_EXPORT type                                     \
    (qtcAtomic##name)(volatile type *atomic, type val)  \
    {                                                   \
        type oldval;                                    \
        pthread_mutex_lock(&__qtc_atomic_lock);         \
        oldval = *atomic;                               \
        *atomic = oldval op val;                        \
        pthread_mutex_unlock(&__qtc_atomic_lock);       \
        return oldval;                                  \
    }
#endif

QTC_DEFINE_ATOMIC(Add, +, int32_t)
QTC_DEFINE_ATOMIC(And, &, uint32_t)
QTC_DEFINE_ATOMIC(Or, |, uint32_t)
QTC_DEFINE_ATOMIC(Xor, ^, uint32_t)

#undef QTC_DEFINE_ATOMIC

// Return the position of the element that is greater than or equal to the key.
// If the key is greater than all the elements in the array, the returned
// pointer will point to the end of the list, i.e. out of the bound of the list.
QTC_EXPORT void*
qtcBSearch(const void *key, const void *base, size_t nmemb, size_t size,
           int (*compar)(const void*, const void*))
{
    size_t l = 0;
    size_t u = nmemb;
    while (l < u) {
        size_t idx = (l + u) / 2;
        const void *p = ((const char*)base) + (idx * size);
        int comparison = (*compar)(key, p);
        if (comparison == 0) {
            l = idx;
            break;
        } else if (comparison < 0) {
            u = idx;
        } else if (comparison > 0) {
            l = idx + 1;
        }
    }
    return (void*)(((const char*)base) + (l * size));
}

static int
qtcStrMapItemCompare(const void *_left, const void *_right, void *_map)
{
    const QtcStrMapItem *left = (const QtcStrMapItem*)_left;
    const QtcStrMapItem *right = (const QtcStrMapItem*)_right;
    QtcStrMap *map = (QtcStrMap*)_map;
    if (map->case_sensitive) {
        return strcmp(left->key, right->key);
    } else {
        return strcasecmp(left->key, right->key);
    }
}

QTC_EXPORT void
qtcStrMapInit(QtcStrMap *map)
{
    if (qtcUnlikely(!map || map->inited || !map->items))
        return;
    if (map->auto_val) {
        for (unsigned i = 0;i < map->num;i++) {
            map->items[i].val = i;
        }
    }
    qsort_r(map->items, map->num, sizeof(QtcStrMapItem),
            qtcStrMapItemCompare, map);
    map->inited = true;
}

typedef struct {
    const char *key;
    bool case_sensitive;
} QtcStrMapCompKey;

static int
qtcStrMapItemCompKey(const void *_key, const void *_item)
{
    const QtcStrMapCompKey *key = (const QtcStrMapCompKey*)_key;
    const QtcStrMapItem *item = (const QtcStrMapItem*)_item;
    if (key->case_sensitive) {
        return strcmp(key->key, item->key);
    } else {
        return strcasecmp(key->key, item->key);
    }
}

QTC_EXPORT int
(qtcStrMapSearch)(const QtcStrMap *map, const char *key, int def)
{
    if (!key || !map) {
        return def;
    }
    QtcStrMapCompKey comp_key = {
        .key = key,
        .case_sensitive = map->case_sensitive
    };
    QtcStrMapItem *item = bsearch(&comp_key, map->items, map->num,
                                  sizeof(QtcStrMapItem), qtcStrMapItemCompKey);
    return item ? item->val : def;
}
