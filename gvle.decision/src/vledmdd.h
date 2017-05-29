/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2016 INRA http://www.inra.fr
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
#ifndef GVLE_VLEOMOD_H
#define GVLE_VLEOMOD_H

#include <QLabel>
#include <QMouseEvent>
#include <QString>
#include <QWidget>
#include <QPainter>
#include <QPoint>
#include <QStaticText>
#include <QStyleOption>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNamedNodeMap>
#include <QXmlDefaultHandler>
#include <vle/value/Value.hpp>
#include <vle/value/Map.hpp>
#include <vle/gvle/dom_tools.hpp>

namespace gvle {
namespace decision {

enum MODIF_TYPE
{
    RENAME, //modif related to the renaming of an object
    ADVANCED_EDTION,
    DEPENDENCY,
    NUMERICAL_INT, //modif concerning numerical integration part
    MOVE_OBJ, //modif concerning moving an obect
    OTHER
};

/**
 * @brief Class that implements vleDomObject especially for vleDm
 */
class vleDomDmDD : public vle::gvle::DomObject
{
public:
    vleDomDmDD(QDomDocument* doc);
    ~vleDomDmDD();
    QString  getXQuery(QDomNode node);
    QDomNode getNodeFromXQuery(const QString& query, QDomNode d=QDomNode());
};

class vleDmDD : public QObject
{
    Q_OBJECT
public:
    vleDmDD(const QString& srcpath, const QString& smpath,
            QString pluginName);
    QString getSrcPath() const
    {return mFileNameSrc;};
    QString getDmPath() const
    {return mFileNameDm;};
    void setSrcPath(const QString& name)
    {mFileNameSrc = name;};
    void setDmPath(const QString& name)
    {mFileNameDm = name;};
    QString toQString(const QDomNode& node) const;
    void xCreateDom();
    void xReadDom();
    QString getDataModel();
    void setCurrentTab(QString tabName);

    void setClassNameToDoc(const QString& className, bool snap);
    void setPluginNameToDoc(const QString& plugName, bool snap);
    void setDataNameToDoc(const QString& dataName, bool snap);
    void setDataPackageToDoc(const QString& pacName, bool snap);
    void setPackageToDoc(const QString& nm, bool snap);

    void save();

    void undo();
    void redo();

    // to factorize
    const QDomDocument& getDomDoc() const
    { return *mDocDm; }

    QDomDocument& getDomDoc()
    { return *mDocDm; }

private:
    QDomNode nodeVariable(const QString& varName);

public slots:
    void onUndoRedoDm(QDomNode oldValDm, QDomNode newValDm);
    void onUndoAvailable(bool);
signals:
    void undoRedo(QDomNode oldValDm, QDomNode newValDm);
    void undoAvailable(bool);

    void modified(int modifiedType);


private:


    QDomDocument*               mDocDm;
    QString                     mFileNameSrc;
    QString                     mFileNameDm;
    vleDomDmDD*                 mVdoDm;
    vle::gvle::DomDiffStack* undoStackDm;
    bool                        waitUndoRedoDm;
    QDomNode                    oldValDm;
    QDomNode                    newValDm;
    QString                     mpluginName;
};

}}//namespaces

#endif
