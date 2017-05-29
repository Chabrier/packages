/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2015 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QMenu>
#include <vle/gvle/gvle_widgets.h>

#include "DecisionPanel.h"
//#include "ui_leftWidget.h"
//#include "ui_rightWidget.h"

namespace vle {
namespace gvle {

DecisionPanel::DecisionPanel():
    PluginMainPanel(),  m_edit(0), m_file(""), dataMetadata(0)
{
    m_edit = new QTextEdit();
    QObject::connect(m_edit, SIGNAL(undoAvailable(bool)),
                     this, SLOT(onUndoAvailable(bool)));
}

DecisionPanel::~DecisionPanel()
{
    //delete right;
    //delete left;
}

QString
DecisionPanel::getname()
{
    return "decision";
}

QWidget*
DecisionPanel::leftWidget()
{
    return m_edit;
}

QWidget*
DecisionPanel::rightWidget()
{
    return 0;
}

void
DecisionPanel::undo()
{
    dataMetadata->undo();
    //reload();
}
void
DecisionPanel::redo()
{
    dataMetadata->redo();
    //reload();
}

void
DecisionPanel::init(const gvle_file& gf, vle::utils::Package* pkg, Logger*,
                     gvle_plugins*, const vle::utils::ContextPtr&)
{
    QString basepath = pkg->getDir(vle::utils::PKG_SOURCE).c_str();
    m_file = gf.source_file;

    dataMetadata = new vleDmDD(gf.source_file,
                               gf.metadata_file, getname());
    dataMetadata->setPackageToDoc("vle.discrete-time.decision", false);
    dataMetadata->setClassNameToDoc("agentDTG", false);
    dataMetadata->save();

    QFile dataFile (m_file);

    if (!dataFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << " warning DecisionPanel::init datafile does not exist";
    }

    QTextStream in(&dataFile);
    m_edit->setText(in.readAll());

}

QString
DecisionPanel::canBeClosed()
{
    return "";
}

void
DecisionPanel::save()
{
    dataMetadata->save();

    if (m_file != "") {
        QFile file(m_file);
        if (file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
            file.write(m_edit->toPlainText().toStdString().c_str()) ;
            file.flush();
            file.close();
        }
    }
    emit undoAvailable(false);
}

PluginMainPanel*
DecisionPanel::newInstance()
{
    return new DecisionPanel;
}

void
DecisionPanel::onUndoAvailable(bool b)
{
    emit undoAvailable(b);
}

}} //namespaces
