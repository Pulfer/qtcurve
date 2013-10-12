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

#ifndef _QTC_UTILS_CONF_FILE_H_
#define _QTC_UTILS_CONF_FILE_H_

#include "conf-desc.h"

typedef union {
    union {
        char *_alloc;
        char _static[0];
    } str_val;
    int int_val;
    int enum_val;
    double float_val;
    bool bool_val;
    QtcColor color_val;
    struct {
        int len;
        union {
            char **_vals;
            char *_buff1[0];
            char _buff2[0];
        };
    } str_list_val;
    struct {
        int len;
        union {
            int *_vals;
            int _buff[0];
        };
    } int_list_val;
    struct {
        int len;
        union {
            double *_vals;
            double _buff[0];
        };
    } float_list_val;
} QtcConfValue;

QTC_BEGIN_DECLS

size_t qtcConfValueSize(const QtcConfValueDesc *desc);
static inline QtcConfValue*
qtcConfValueNew(const QtcConfValueDesc *desc)
{
    return qtcNewSize(QtcConfValue, qtcConfValueSize(desc));
}
void qtcConfValueLoad(QtcConfValue *value, const char *str,
                      const QtcConfValueDesc *desc);
void qtcConfValueDone(QtcConfValue *value, const QtcConfValueDesc *desc);
void qtcConfValueFree(QtcConfValue *value);

QTC_END_DECLS

#endif
