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

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlParentChangePrivate;
class Q_DECLARATIVE_EXPORT QmlParentChange : public QmlStateOperation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlParentChange)

    Q_PROPERTY(QObject *target READ object WRITE setObject)
    Q_PROPERTY(QObject *parent READ parent WRITE setParent)
public:
    QmlParentChange(QObject *parent=0);
    ~QmlParentChange();

    QObject *object() const;
    void setObject(QObject *);

    QObject *parent() const;
    void setParent(QObject *);

    virtual ActionList actions();
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

class QmlSetPropertyPrivate;
class Q_DECLARATIVE_EXPORT QmlSetProperty : public QmlStateOperation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlSetProperty)

    Q_PROPERTY(QObject *target READ object WRITE setObject)
    Q_PROPERTY(QString property READ property WRITE setProperty)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
    Q_PROPERTY(QString binding READ binding WRITE setBinding)

public:
    QmlSetProperty(QObject *parent=0);
    ~QmlSetProperty();

    QObject *object();
    void setObject(QObject *);
    QString property() const;
    void setProperty(const QString &);
    QVariant value() const;
    void setValue(const QVariant &);
    QString binding() const;
    void setBinding(const QString&);

    virtual ActionList actions();
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlParentChange)
QML_DECLARE_TYPE(QmlRunScript)
QML_DECLARE_TYPE(QmlSetProperty)

QT_END_HEADER

#endif // QMLSTATEOPERATIONS_H
