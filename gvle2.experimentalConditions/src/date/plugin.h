/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2015 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GVLE2_CONDDATEPLUGIN_H
#define GVLE2_CONDDATEPLUGIN_H

#include <QObject>
#include <QSettings>
#include <vle/utils/Package.hpp>
#include <vle/gvle2/plugin_cond.h>
#include <vle/gvle2/logger.h>
#include <vle/gvle2/vlevpm.h>
#include "tab.h"
#include "toolbar.h"

namespace vle{
namespace gvle2{

class CondDatePlugin : public PluginExpCond
{
    Q_OBJECT
    Q_INTERFACES(vle::gvle2::PluginExpCond)

public:
    CondDatePlugin();
    ~CondDatePlugin();
    QString getname();
    QWidget *getWidget();
    void     delWidget();
    QWidget *getWidgetToolbar();
    void     delWidgetToolbar();
    void init(vleVpm*vpm, const QString& cond);
    void setSettings(QSettings *s);
    void setLogger(Logger *logger);
    void setPackage(vle::utils::Package *pkg);

public slots:
    void onTabDeleted(QObject *obj);

private:
    QSettings*  mSettings;
    Logger*     mLogger;
    MainTab*    mWidgetTab;
    widToolbar* mWidgetToolbar;
    QString     mExpCond;
    vleVpm*     mVpm;
};

}}//namespaces

#endif // EXPCOND_DUMMY_H
