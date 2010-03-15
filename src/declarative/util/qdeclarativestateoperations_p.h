/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QDECLARATIVESTATEOPERATIONS_H
#define QDECLARATIVESTATEOPERATIONS_H

#include "qdeclarativestate_p.h"

#include <qdeclarativeitem.h>
#include <private/qdeclarativeanchors_p.h>
#include <qdeclarativescriptstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeParentChangePrivate;
class Q_DECLARATIVE_EXPORT QDeclarativeParentChange : public QDeclarativeStateOperation, public QDeclarativeActionEvent
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeParentChange)

    Q_PROPERTY(QDeclarativeItem *target READ object WRITE setObject)
    Q_PROPERTY(QDeclarativeItem *parent READ parent WRITE setParent)
    Q_PROPERTY(qreal x READ x WRITE setX)
    Q_PROPERTY(qreal y READ y WRITE setY)
    Q_PROPERTY(qreal width READ width WRITE setWidth)
    Q_PROPERTY(qreal height READ height WRITE setHeight)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)
public:
    QDeclarativeParentChange(QObject *parent=0);
    ~QDeclarativeParentChange();

    QDeclarativeItem *object() const;
    void setObject(QDeclarativeItem *);

    QDeclarativeItem *parent() const;
    void setParent(QDeclarativeItem *);

    QDeclarativeItem *originalParent() const;

    qreal x() const;
    void setX(qreal x);
    bool xIsSet() const;

    qreal y() const;
    void setY(qreal y);
    bool yIsSet() const;

    qreal width() const;
    void setWidth(qreal width);
    bool widthIsSet() const;

    qreal height() const;
    void setHeight(qreal height);
    bool heightIsSet() const;

    qreal scale() const;
    void setScale(qreal scale);
    bool scaleIsSet() const;

    qreal rotation() const;
    void setRotation(qreal rotation);
    bool rotationIsSet() const;

    virtual ActionList actions();

    virtual void saveOriginals();
    virtual void copyOriginals(QDeclarativeActionEvent*);
    virtual void execute();
    virtual bool isReversable();
    virtual void reverse();
    virtual QString typeName() const;
    virtual bool override(QDeclarativeActionEvent*other);
    virtual void rewind();
    virtual void saveCurrentValues();
};

class QDeclarativeStateChangeScriptPrivate;
class Q_DECLARATIVE_EXPORT QDeclarativeStateChangeScript : public QDeclarativeStateOperation, public QDeclarativeActionEvent
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeStateChangeScript)

    Q_PROPERTY(QDeclarativeScriptString script READ script WRITE setScript)
    Q_PROPERTY(QString name READ name WRITE setName)

public:
    QDeclarativeStateChangeScript(QObject *parent=0);
    ~QDeclarativeStateChangeScript();

    virtual ActionList actions();

    virtual QString typeName() const;

    QDeclarativeScriptString script() const;
    void setScript(const QDeclarativeScriptString &);
    
    QString name() const;
    void setName(const QString &);

    virtual void execute();
};

class QDeclarativeAnchorChangesPrivate;
class Q_DECLARATIVE_EXPORT QDeclarativeAnchorChanges : public QDeclarativeStateOperation, public QDeclarativeActionEvent
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeAnchorChanges)

    Q_PROPERTY(QDeclarativeItem *target READ object WRITE setObject)
    Q_PROPERTY(QString reset READ reset WRITE setReset)
    Q_PROPERTY(QDeclarativeAnchorLine left READ left WRITE setLeft)
    Q_PROPERTY(QDeclarativeAnchorLine right READ right WRITE setRight)
    Q_PROPERTY(QDeclarativeAnchorLine horizontalCenter READ horizontalCenter WRITE setHorizontalCenter)
    Q_PROPERTY(QDeclarativeAnchorLine top READ top WRITE setTop)
    Q_PROPERTY(QDeclarativeAnchorLine bottom READ bottom WRITE setBottom)
    Q_PROPERTY(QDeclarativeAnchorLine verticalCenter READ verticalCenter WRITE setVerticalCenter)
    Q_PROPERTY(QDeclarativeAnchorLine baseline READ baseline WRITE setBaseline)

public:
    QDeclarativeAnchorChanges(QObject *parent=0);
    ~QDeclarativeAnchorChanges();

    virtual ActionList actions();

    QDeclarativeItem *object() const;
    void setObject(QDeclarativeItem *);

    QString reset() const;
    void setReset(const QString &);

    QDeclarativeAnchorLine left() const;
    void setLeft(const QDeclarativeAnchorLine &edge);

    QDeclarativeAnchorLine right() const;
    void setRight(const QDeclarativeAnchorLine &edge);

    QDeclarativeAnchorLine horizontalCenter() const;
    void setHorizontalCenter(const QDeclarativeAnchorLine &edge);

    QDeclarativeAnchorLine top() const;
    void setTop(const QDeclarativeAnchorLine &edge);

    QDeclarativeAnchorLine bottom() const;
    void setBottom(const QDeclarativeAnchorLine &edge);

    QDeclarativeAnchorLine verticalCenter() const;
    void setVerticalCenter(const QDeclarativeAnchorLine &edge);

    QDeclarativeAnchorLine baseline() const;
    void setBaseline(const QDeclarativeAnchorLine &edge);

    virtual void execute();
    virtual bool isReversable();
    virtual void reverse();
    virtual QString typeName() const;
    virtual bool override(QDeclarativeActionEvent*other);
    virtual bool changesBindings();
    virtual void saveOriginals();
    virtual void copyOriginals(QDeclarativeActionEvent*);
    virtual void clearBindings();
    virtual void rewind();
    virtual void saveCurrentValues();

    QList<QDeclarativeAction> additionalActions();
    virtual void saveTargetValues();
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeParentChange)
QML_DECLARE_TYPE(QDeclarativeStateChangeScript)
QML_DECLARE_TYPE(QDeclarativeAnchorChanges)

QT_END_HEADER

#endif // QDECLARATIVESTATEOPERATIONS_H
