/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMLSTATEOPERATIONS_H
#define QMLSTATEOPERATIONS_H

#include <QtDeclarative/qmlstate.h>
#include <QtDeclarative/qfxitem.h>
#include <QtDeclarative/qfxanchors.h>
#include <QtDeclarative/qmlscriptstring.h>

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

    virtual void saveOriginals();
    virtual void execute();
    virtual bool isReversable();
    virtual void reverse();
    virtual QString typeName() const;
    virtual bool override(ActionEvent*other);
};

class QmlStateChangeScriptPrivate;
class Q_DECLARATIVE_EXPORT QmlStateChangeScript : public QmlStateOperation, public ActionEvent
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlStateChangeScript)

    Q_PROPERTY(QmlScriptString script READ script WRITE setScript)
    Q_PROPERTY(QString name READ name WRITE setName)

public:
    QmlStateChangeScript(QObject *parent=0);
    ~QmlStateChangeScript();

    virtual ActionList actions();

    QmlScriptString script() const;
    void setScript(const QmlScriptString &);
    
    QString name() const;
    void setName(const QString &);

    virtual void execute();
};

class QmlAnchorChangesPrivate;
class Q_DECLARATIVE_EXPORT QmlAnchorChanges : public QmlStateOperation, public ActionEvent
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlAnchorChanges)

    Q_PROPERTY(QFxItem *target READ object WRITE setObject)
    Q_PROPERTY(QString reset READ reset WRITE setReset)
    Q_PROPERTY(QFxAnchorLine left READ left WRITE setLeft)
    Q_PROPERTY(QFxAnchorLine right READ right WRITE setRight)
    Q_PROPERTY(QFxAnchorLine horizontalCenter READ horizontalCenter WRITE setHorizontalCenter)
    Q_PROPERTY(QFxAnchorLine top READ top WRITE setTop)
    Q_PROPERTY(QFxAnchorLine bottom READ bottom WRITE setBottom)
    Q_PROPERTY(QFxAnchorLine verticalCenter READ verticalCenter WRITE setVerticalCenter)
    Q_PROPERTY(QFxAnchorLine baseline READ baseline WRITE setBaseline)

public:
    QmlAnchorChanges(QObject *parent=0);
    ~QmlAnchorChanges();

    virtual ActionList actions();

    QFxItem *object() const;
    void setObject(QFxItem *);

    QString reset() const;
    void setReset(const QString &);

    QFxAnchorLine left() const;
    void setLeft(const QFxAnchorLine &edge);

    QFxAnchorLine right() const;
    void setRight(const QFxAnchorLine &edge);

    QFxAnchorLine horizontalCenter() const;
    void setHorizontalCenter(const QFxAnchorLine &edge);

    QFxAnchorLine top() const;
    void setTop(const QFxAnchorLine &edge);

    QFxAnchorLine bottom() const;
    void setBottom(const QFxAnchorLine &edge);

    QFxAnchorLine verticalCenter() const;
    void setVerticalCenter(const QFxAnchorLine &edge);

    QFxAnchorLine baseline() const;
    void setBaseline(const QFxAnchorLine &edge);

    virtual void execute();
    virtual bool isReversable();
    virtual void reverse();
    virtual QString typeName() const;
    virtual bool override(ActionEvent*other);
    virtual QList<Action> extraActions();
    virtual bool changesBindings();
    virtual void saveOriginals();
    virtual void clearForwardBindings();
    virtual void clearReverseBindings();
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlParentChange)
QML_DECLARE_TYPE(QmlStateChangeScript)
QML_DECLARE_TYPE(QmlAnchorChanges)

QT_END_HEADER

#endif // QMLSTATEOPERATIONS_H
