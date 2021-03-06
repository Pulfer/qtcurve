/*
  QtCurve KWin window decoration
  Copyright (C) 2007 - 2010 Craig Drummond <craig.p.drummond@gmail.com>

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

#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdeversion.h>
#include <QDBusConnection>
#include "config.h"
#include <common/common.h>
#include "qtcurvekwinconfig.h"

static void insertColorEntries(QComboBox *combo)
{
    combo->insertItem(KWinQtCurve::QtCurveShadowConfiguration::CT_FOCUS, i18n("Focus"));
    combo->insertItem(KWinQtCurve::QtCurveShadowConfiguration::CT_HOVER, i18n("Hover"));
    combo->insertItem(KWinQtCurve::QtCurveShadowConfiguration::CT_SELECTION, i18n("Selection Background"));
    combo->insertItem(KWinQtCurve::QtCurveShadowConfiguration::CT_TITLEBAR, i18n("Titlebar"));
    combo->insertItem(KWinQtCurve::QtCurveShadowConfiguration::CT_GRAY, i18n("Gray"));
    combo->insertItem(KWinQtCurve::QtCurveShadowConfiguration::CT_CUSTOM, i18n("Custom:"));
}

static void insertSizeEntries(QComboBox *combo)
{
    combo->insertItem(KWinQtCurve::QtCurveConfig::BORDER_NONE, i18n("No Border"));
    combo->insertItem(KWinQtCurve::QtCurveConfig::BORDER_NO_SIDES, i18n("No Side Border"));
    combo->insertItem(KWinQtCurve::QtCurveConfig::BORDER_TINY, i18n("Tiny"));
    combo->insertItem(KWinQtCurve::QtCurveConfig::BORDER_NORMAL, i18n("Normal"));
    combo->insertItem(KWinQtCurve::QtCurveConfig::BORDER_LARGE, i18n("Large"));
    combo->insertItem(KWinQtCurve::QtCurveConfig::BORDER_VERY_LARGE, i18n("Very Large"));
    combo->insertItem(KWinQtCurve::QtCurveConfig::BORDER_HUGE, i18n("Huge"));
    combo->insertItem(KWinQtCurve::QtCurveConfig::BORDER_VERY_HUGE, i18n("Very Huge"));
    combo->insertItem(KWinQtCurve::QtCurveConfig::BORDER_OVERSIZED, i18n("Oversized"));
}

static void insertShadeEntries(QComboBox *combo)
{
    combo->insertItem(KWinQtCurve::QtCurveConfig::SHADE_NONE, i18n("None"));
    combo->insertItem(KWinQtCurve::QtCurveConfig::SHADE_DARK, i18n("Dark"));
    combo->insertItem(KWinQtCurve::QtCurveConfig::SHADE_LIGHT, i18n("Light"));
    combo->insertItem(KWinQtCurve::QtCurveConfig::SHADE_SHADOW, i18n("Shadow"));
}

static const char * constDBusService="org.kde.kcontrol.QtCurve";

QtCurveKWinConfig::QtCurveKWinConfig(KConfig *config, QWidget *parent)
                 : QWidget(parent)
                 , itsActiveShadows(QPalette::Active)
                 , itsInactiveShadows(QPalette::Inactive)
{
    Q_UNUSED(config);

    KGlobal::locale()->insertCatalog("qtcurve");
    KGlobal::locale()->insertCatalog("kwin_clients");

    if(!QDBusConnection::sessionBus().registerService(constDBusService))
    {
        itsOk=false;
        QBoxLayout *layout=new QBoxLayout(QBoxLayout::TopToBottom, this);
        layout->addWidget(new QLabel(i18n("<h3>Already Open</h3><p>Another QtCurve configuration dialog is already open. "
                                          "Please close the other before proceeding."), this));
    }
    else
    {
        itsOk=true;

        setupUi(this);

        insertSizeEntries(borderSize);
        insertColorEntries(activeShadowColorType);
        insertColorEntries(inactiveShadowColorType);
        insertShadeEntries(outerBorder);
        insertShadeEntries(innerBorder);

        load(0L);
        connect(borderSize, SIGNAL(currentIndexChanged(int)), this, SLOT(sizeChanged()));
        connect(roundBottom, SIGNAL(toggled(bool)), this, SIGNAL(changed()));
        connect(outerBorder, SIGNAL(currentIndexChanged(int)), this, SLOT(outerBorderChanged()));
        connect(innerBorder, SIGNAL(currentIndexChanged(int)), this, SLOT(innerBorderChanged()));
        connect(borderlessMax, SIGNAL(toggled(bool)), this, SIGNAL(changed()));
        connect(titleBarPad, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
        connect(edgePad, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
        titleBarPad->setRange(-5, 10);
        edgePad->setRange(0, 10);
        connect(useShadows, SIGNAL(toggled(bool)), this, SLOT(shadowsChanged()));
        connect(activeShadowSize, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
        connect(activeShadowHOffset, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
        connect(activeShadowVOffset, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
        connect(activeShadowColorType, SIGNAL(currentIndexChanged(int)), this, SLOT(activeShadowColorTypeChanged()));
        connect(activeShadowColor, SIGNAL(changed(const QColor &)), this, SIGNAL(changed()));
        connect(inactiveShadowSize, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
        connect(inactiveShadowHOffset, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
        connect(inactiveShadowVOffset, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
        connect(inactiveShadowColorType, SIGNAL(currentIndexChanged(int)), this, SLOT(inactiveShadowColorTypeChanged()));
        connect(inactiveShadowColor, SIGNAL(changed(const QColor &)), this, SIGNAL(changed()));
        connect(inactiveUsesActiveGradients, SIGNAL(toggled(bool)), this, SIGNAL(changed()));
        activeShadowColorTypeChanged();
        inactiveShadowColorTypeChanged();
        activeShadowSize->setRange(KWinQtCurve::QtCurveShadowConfiguration::MIN_SIZE,
                                            KWinQtCurve::QtCurveShadowConfiguration::MAX_SIZE);
        inactiveShadowSize->setRange(KWinQtCurve::QtCurveShadowConfiguration::MIN_SIZE,
                                                KWinQtCurve::QtCurveShadowConfiguration::MAX_SIZE);
        activeShadowHOffset->setRange(KWinQtCurve::QtCurveShadowConfiguration::MIN_OFFSET,
                                                KWinQtCurve::QtCurveShadowConfiguration::MAX_OFFSET);
        inactiveShadowHOffset->setRange(KWinQtCurve::QtCurveShadowConfiguration::MIN_OFFSET,
                                                KWinQtCurve::QtCurveShadowConfiguration::MAX_OFFSET);
        activeShadowVOffset->setRange(KWinQtCurve::QtCurveShadowConfiguration::MIN_OFFSET,
                                                KWinQtCurve::QtCurveShadowConfiguration::MAX_OFFSET);
        inactiveShadowVOffset->setRange(KWinQtCurve::QtCurveShadowConfiguration::MIN_OFFSET,
                                                KWinQtCurve::QtCurveShadowConfiguration::MAX_OFFSET);
        setShadows();

#if KDE_IS_VERSION(4, 8, 80)
        grouping->setVisible(false);
        groupingLabel->setVisible(false);
#else
        connect(grouping, SIGNAL(toggled(bool)), this, SIGNAL(changed()));
#endif
        connect(activeOpacity, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
        connect(inactiveOpacity, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
        connect(opaqueBorder, SIGNAL(toggled(bool)), this, SIGNAL(changed()));
    }
}

QtCurveKWinConfig::~QtCurveKWinConfig()
{
    if(itsOk)
        QDBusConnection::sessionBus().unregisterService(constDBusService);
}

void QtCurveKWinConfig::load(const KConfigGroup &)
{
    load(0L);
}

void QtCurveKWinConfig::load(KConfig *c)
{
    if(!itsOk)
        return;

    KConfig *cfg=c ? c : new KConfig("kwinqtcurverc");

    itsActiveShadows.load(cfg);
    itsInactiveShadows.load(cfg);
    setShadows();
    KWinQtCurve::QtCurveConfig config;

    config.load(cfg, c ? KWIN_GROUP : 0L);
    setWidgets(config);
    if(!c)
        delete cfg;
}

void QtCurveKWinConfig::save(KConfigGroup &)
{
    save(0L);
}

void QtCurveKWinConfig::save(KConfig *c)
{
    if(!itsOk)
        return;

    KConfig *cfg=c ? c : new KConfig("kwinqtcurverc");

    KWinQtCurve::QtCurveConfig config;

    config.setBorderSize((KWinQtCurve::QtCurveConfig::Size)borderSize->currentIndex());
    config.setRoundBottom(roundBottom->isChecked());
    config.setOuterBorder((KWinQtCurve::QtCurveConfig::Shade)outerBorder->currentIndex());
    config.setInnerBorder((KWinQtCurve::QtCurveConfig::Shade)innerBorder->currentIndex());
    config.setBorderlessMax(borderlessMax->isChecked());
    config.setTitleBarPad(titleBarPad->value());
    config.setEdgePad(edgePad->value());

    config.setCustomShadows(useShadows->isChecked());
    if(useShadows->isChecked())
    {
        itsActiveShadows.setShadowSize(activeShadowSize->value());
        itsActiveShadows.setHorizontalOffset(activeShadowHOffset->value());
        itsActiveShadows.setVerticalOffset(activeShadowVOffset->value());
        itsActiveShadows.setColorType((KWinQtCurve::QtCurveShadowConfiguration::ColorType)activeShadowColorType->currentIndex());
        if(KWinQtCurve::QtCurveShadowConfiguration::CT_CUSTOM==activeShadowColorType->currentIndex())
            itsActiveShadows.setColor(activeShadowColor->color());
        itsInactiveShadows.setShadowSize(inactiveShadowSize->value());
        itsInactiveShadows.setHorizontalOffset(inactiveShadowHOffset->value());
        itsInactiveShadows.setVerticalOffset(inactiveShadowVOffset->value());
        itsInactiveShadows.setColorType((KWinQtCurve::QtCurveShadowConfiguration::ColorType)inactiveShadowColorType->currentIndex());
        itsInactiveShadows.setShadowType(inactiveUsesActiveGradients->isChecked()
                                            ? KWinQtCurve::QtCurveShadowConfiguration::SH_ACTIVE
                                            : KWinQtCurve::QtCurveShadowConfiguration::SH_INACTIVE);
        if(KWinQtCurve::QtCurveShadowConfiguration::CT_CUSTOM==inactiveShadowColorType->currentIndex())
            itsInactiveShadows.setColor(inactiveShadowColor->color());
    }
    else
    {
        itsActiveShadows.defaults();
        itsInactiveShadows.defaults();
    }
    itsActiveShadows.save(cfg);
    itsInactiveShadows.save(cfg);
#if !KDE_IS_VERSION(4, 8, 80)
    config.setGrouping(grouping->isChecked());
#endif
    config.setOpacity(activeOpacity->value(), true);
    config.setOpacity(inactiveOpacity->value(), false);
    config.setOpaqueBorder(opaqueBorder->isChecked());
    config.save(cfg, c ? KWIN_GROUP : 0L);
    cfg->sync();
    if(!c)
        delete cfg;
}

void QtCurveKWinConfig::defaults()
{
    if(!itsOk)
        return;

    setWidgets(KWinQtCurve::QtCurveConfig());
    itsActiveShadows.defaults();
    itsInactiveShadows.defaults();
    setShadows();
}

void QtCurveKWinConfig::outerBorderChanged()
{
    if(KWinQtCurve::QtCurveConfig::SHADE_NONE==outerBorder->currentIndex())
        innerBorder->setCurrentIndex(KWinQtCurve::QtCurveConfig::SHADE_NONE);
    if(KWinQtCurve::QtCurveConfig::SHADE_SHADOW==outerBorder->currentIndex())
        useShadows->setChecked(true);
    setWidgetStates();
    emit changed();
}

void QtCurveKWinConfig::innerBorderChanged()
{
    if(KWinQtCurve::QtCurveConfig::SHADE_NONE!=innerBorder->currentIndex() &&
       KWinQtCurve::QtCurveConfig::SHADE_NONE==outerBorder->currentIndex())
        outerBorder->setCurrentIndex(innerBorder->currentIndex());
    if(KWinQtCurve::QtCurveConfig::SHADE_SHADOW==outerBorder->currentIndex())
        useShadows->setChecked(true);
    emit changed();
}

void QtCurveKWinConfig::shadowsChanged()
{
    if(!useShadows->isChecked())
    {
        if(KWinQtCurve::QtCurveConfig::SHADE_SHADOW==outerBorder->currentIndex())
            outerBorder->setCurrentIndex(KWinQtCurve::QtCurveConfig::SHADE_DARK);
        if(KWinQtCurve::QtCurveConfig::SHADE_SHADOW==innerBorder->currentIndex())
            innerBorder->setCurrentIndex(KWinQtCurve::QtCurveConfig::SHADE_DARK);
    }
    emit changed();
}

void QtCurveKWinConfig::activeShadowColorTypeChanged()
{
    activeShadowColor->setEnabled(KWinQtCurve::QtCurveShadowConfiguration::CT_CUSTOM==activeShadowColorType->currentIndex());
    if(itsActiveShadows.colorType()!=activeShadowColorType->currentIndex())
        emit changed();
}

void QtCurveKWinConfig::inactiveShadowColorTypeChanged()
{
    inactiveShadowColor->setEnabled(KWinQtCurve::QtCurveShadowConfiguration::CT_CUSTOM==inactiveShadowColorType->currentIndex());
    if(itsInactiveShadows.colorType()!=inactiveShadowColorType->currentIndex())
        emit changed();
}

void QtCurveKWinConfig::sizeChanged()
{
    setWidgetStates();
    emit changed();
}

void QtCurveKWinConfig::setWidgets(const KWinQtCurve::QtCurveConfig &cfg)
{
    borderSize->setCurrentIndex(cfg.borderSize());
    roundBottom->setChecked(cfg.roundBottom());
    outerBorder->setCurrentIndex(cfg.outerBorder());
    innerBorder->setCurrentIndex(cfg.innerBorder());
    borderlessMax->setChecked(cfg.borderlessMax());
    titleBarPad->setValue(cfg.titleBarPad());
    edgePad->setValue(cfg.edgePad());
    useShadows->setChecked(cfg.customShadows());
#if !KDE_IS_VERSION(4, 8, 80)
    grouping->setChecked(cfg.grouping());
#endif
    activeOpacity->setValue(cfg.opacity(true));
    inactiveOpacity->setValue(cfg.opacity(false));
    opaqueBorder->setChecked(cfg.opaqueBorder());
    setWidgetStates();
}

void QtCurveKWinConfig::setWidgetStates()
{
    if(KWinQtCurve::QtCurveConfig::BORDER_NONE==borderSize->currentIndex())
    {
        roundBottom->setEnabled(false);
        roundBottom->setChecked(false);
    }
    else
        roundBottom->setEnabled(true);

    if(KWinQtCurve::QtCurveConfig::SHADE_NONE==outerBorder->currentIndex() ||
       borderSize->currentIndex()<KWinQtCurve::QtCurveConfig::BORDER_TINY)
    {
        innerBorder->setEnabled(false);
        innerBorder->setCurrentIndex(KWinQtCurve::QtCurveConfig::SHADE_NONE);
    }
    else
        innerBorder->setEnabled(true);
}

void QtCurveKWinConfig::setNote(const QString &txt)
{
    noteLabel->setText(txt);
}

void QtCurveKWinConfig::setShadows()
{
    activeShadowSize->setValue(itsActiveShadows.shadowSize());
    activeShadowHOffset->setValue(itsActiveShadows.horizontalOffset());
    activeShadowVOffset->setValue(itsActiveShadows.verticalOffset());
    activeShadowColor->setColor(itsActiveShadows.color());
    activeShadowColorType->setCurrentIndex(itsActiveShadows.colorType());
    inactiveShadowSize->setValue(itsInactiveShadows.shadowSize());
    inactiveShadowHOffset->setValue(itsInactiveShadows.horizontalOffset());
    inactiveShadowVOffset->setValue(itsInactiveShadows.verticalOffset());
    inactiveShadowColor->setColor(itsInactiveShadows.color());
    inactiveShadowColorType->setCurrentIndex(itsInactiveShadows.colorType());
    inactiveUsesActiveGradients->setChecked(KWinQtCurve::QtCurveShadowConfiguration::SH_ACTIVE==itsInactiveShadows.shadowType());
}

extern "C"
{
    KDE_EXPORT QObject * allocate_config(KConfig *config, QWidget *parent)
    {
        return new QtCurveKWinConfig(config, parent);
    }
}

#include "qtcurvekwinconfig.moc"
