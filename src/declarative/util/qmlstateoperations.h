/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMLSTATEOPERATIONS_H
#define QMLSTATEOPERATIONS_H

#include <QtDeclarative/qmlstate.h>
#include <QtDeclarative/qfxitem.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlParentChangePrivate;
class Q_DECLARATIVE_EXPORT QmlParentChange : public QmlStateOperation, public ActionEvent
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlParentChange)

    Q_PROPERTY(QFxItem *target READ object WRITE setObject)
    Q_PROPERTY(QFxItem *parent READ parent WRITE setParent)
public:
    QmlParentChange(QObject *parent=0);
    ~QmlParentChange();

    QFxItem *object() const;
    void setObject(QFxItem *);

    QFxItem *parent() const;
    void setParent(QFxItem *);

    virtual ActionList actions();

    virtual void execute();
    virtual bool isReversable();
    virtual void reverse();
};

class QmlRunScriptPrivate;
class Q_DECLARATIVE_EXPORT QmlRunScript : public QmlStateOperation, public ActionEvent
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlRunScript)

    Q_PROPERTY(QString script READ script WRITE setScript)
    Q_PROPERTY(QString name READ name WRITE setName)

public:
    QmlRunScript(QObject *parent=0);
    ~QmlRunScript();

    virtual ActionList actions();

    QString script() const;
    void setScript(const QString &);
    
    virtual QString name() const;
    void setName(const QString &);

    virtual void execute();
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlParentChange)
QML_DECLARE_TYPE(QmlRunScript)

QT_END_HEADER

#endif // QMLSTATEOPERATIONS_H
