/*
  QtCurve (C) Craig Drummond, 2007 - 2010 craig.p.drummond@gmail.com

  ----

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include "config.h"
#ifdef QTC_ENABLE_X11
#  include <QX11Info>
#  include <qtcurve-utils/x11utils.h>
#endif
#include <qtcurve-utils/log.h>

#include "utils.h"

#ifndef QTC_QT4_ENABLE_KDE
#  undef KDE_IS_VERSION
#  define KDE_IS_VERSION(A, B, C) 0
#else
#  include <kdeversion.h>
#  include <KDE/KWindowSystem>
#endif

namespace QtCurve {
namespace Utils {
bool
compositingActive()
{
#if !defined QTC_QT4_ENABLE_KDE || !KDE_IS_VERSION(4, 4, 0)
#ifdef QTC_ENABLE_X11
    return qtcX11CompositingActive();
#else
    return false;
#endif
#else // QTC_QT4_ENABLE_KDE
    return KWindowSystem::compositingActive();
#endif // QTC_QT4_ENABLE_KDE
}
bool
hasAlphaChannel(const QWidget *widget)
{
    if (compositingActive()) {
#ifdef QTC_ENABLE_X11
        return (32 == (widget ? widget->x11Info().depth() :
                       QX11Info().appDepth()));
#else
        QTC_UNUSED(widget);
        return true;
#endif
    } else {
        return false;
    }
}
}
}
