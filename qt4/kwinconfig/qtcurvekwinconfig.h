/*
  QtCurve KWin window decoration
  Copyright (C) 2007 - 2010 Craig Drummond <craig.p.drummond@gmail.com>

  based on the window decoration "Plastik":
  Copyright (C) 2003-2005 Sandro Giessl <sandro@giessl.com>

  based on the window decoration "Web":
  Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
 */

#ifndef QTCURVE_KWIN_CONFIG_WIDGET_H
#define QTCURVE_KWIN_CONFIG_WIDGET_H

#include <QObject>
#include "ui_qtcurvekwinconfigwidget.h"
#include "../kwin/qtcurveshadowconfiguration.h"
#include "../kwin/qtcurveconfig.h"

class KConfig;

class QtCurveKWinConfig : public QWidget, public Ui::QtCurveKWinConfigWidget
{
    Q_OBJECT

    public:

    QtCurveKWinConfig(KConfig *config, QWidget *parent);
    ~QtCurveKWinConfig();

    bool ok() { return itsOk; }

    Q_SIGNALS:

    void changed();

    public Q_SLOTS:

    void load(const KConfigGroup &);
    void save(KConfigGroup &);
    void defaults();
    void outerBorderChanged();
    void innerBorderChanged();
    void shadowsChanged();
    void activeShadowColorTypeChanged();
    void inactiveShadowColorTypeChanged();
    void sizeChanged();

    public:

    void setNote(const QString &txt);
    void load(KConfig *config);
    void save(KConfig *config);

    private:

    void setShadows();
    void setWidgets(const KWinQtCurve::QtCurveConfig &cfg);
    void setWidgetStates();

    private:

    bool                                    itsOk;
    KWinQtCurve::QtCurveShadowConfiguration itsActiveShadows,
                                            itsInactiveShadows;
};

#endif // KNIFTYCONFIG_H
