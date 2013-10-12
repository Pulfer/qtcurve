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

#include "conf-file.h"

static size_t
_qtcConfValueSize(const QtcConfValueDesc *desc)
{
    const QtcConfConstrain *constrain = &desc->constrain;
    switch (desc->type) {
    default:
    case QTC_CONF_STR:
        if (!constrain->str_c.is_static ||
            qtcUnlikely(constrain->str_c.max_len <= 0))
            return sizeof(char*);
        return constrain->str_c.max_len + 1;
    case QTC_CONF_INT:
    case QTC_CONF_ENUM:
        return sizeof(int);
    case QTC_CONF_FLOAT:
        return sizeof(double);
    case QTC_CONF_BOOL:
        return sizeof(bool);
    case QTC_CONF_COLOR:
        return sizeof(QtcColor);
    case QTC_CONF_STR_LIST:
        if (!constrain->str_list_c.is_array_static ||
            qtcUnlikely(constrain->str_list_c.max_count <= 0))
            return sizeof(char**); // or sizeof(char*)
        if (!constrain->str_list_c.is_str_static ||
            qtcUnlikely(constrain->str_list_c.max_strlen <= 0))
            return sizeof(char*) * constrain->str_list_c.max_count;
        return ((constrain->str_list_c.max_strlen + 1) *
                constrain->str_list_c.max_count);
    case QTC_CONF_INT_LIST:
        if (!constrain->int_list_c.is_array_static ||
            qtcUnlikely(constrain->int_list_c.max_count <= 0))
            return sizeof(int*);
        return (sizeof(int) * constrain->int_list_c.max_count);
    case QTC_CONF_FLOAT_LIST:
        if (!constrain->float_list_c.is_array_static ||
            qtcUnlikely(constrain->float_list_c.max_count <= 0))
            return sizeof(double*);
        return (sizeof(double) * constrain->float_list_c.max_count);
    }
}

QTC_EXPORT size_t
qtcConfValueSize(const QtcConfValueDesc *desc)
{
    return qtcMax(_qtcConfValueSize(desc), sizeof(QtcConfValue));
}
