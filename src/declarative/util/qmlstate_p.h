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

#ifndef QMLSTATE_H
#define QMLSTATE_H

#include <qml.h>

#include <QtCore/qobject.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlActionEvent;
class QmlBinding;
class Q_DECLARATIVE_EXPORT QmlAction
{
public:
    QmlAction();
    QmlAction(QObject *, const QString &, const QVariant &);

    bool restore:1;
    bool actionDone:1;
    bool reverseEvent:1;
    bool deletableToBinding:1;

    QmlMetaProperty property;
    QVariant fromValue;
    QVariant toValue;

    QmlAbstractBinding *fromBinding;
    QmlAbstractBinding *toBinding;
    QmlActionEvent *event;

    //strictly for matching
    QObject *specifiedObject;
    QString specifiedProperty;

    void deleteFromBinding();
};

class QmlActionEvent
{
public:
    virtual ~QmlActionEvent();
    virtual QString typeName() const;

    virtual void execute();
    virtual bool isReversable();
    virtual void reverse();
    virtual void saveOriginals() {}

    virtual bool isRewindable() { return isReversable(); }
    virtual void rewind() {}
    virtual void saveCurrentValues() {}

    //virtual bool hasExtraActions();
    virtual QList<QmlAction> extraActions();

    virtual bool changesBindings();
    virtual void clearForwardBindings();
    virtual void clearReverseBindings();
    virtual bool override(QmlActionEvent*other);
};

//### rename to QmlStateChange?
class QmlStateGroup;
class Q_DECLARATIVE_EXPORT QmlStateOperation : public QObject
{
    Q_OBJECT
public:
    QmlStateOperation(QObject *parent = 0)
        : QObject(parent) {}
    typedef QList<QmlAction> ActionList;

    virtual ActionList actions();

protected:
    QmlStateOperation(QObjectPrivate &dd, QObject *parent = 0);
};

typedef QmlStateOperation::ActionList QmlStateActions;

class QmlTransition;
class QmlStatePrivate;
class Q_DECLARATIVE_EXPORT QmlState : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QmlBinding *when READ when WRITE setWhen)
    Q_PROPERTY(QString extend READ extends WRITE setExtends)
    Q_PROPERTY(QmlListProperty<QmlStateOperation> changes READ changes)
    Q_CLASSINFO("DefaultProperty", "changes")
    Q_CLASSINFO("DeferredPropertyNames", "changes")

public:
    QmlState(QObject *parent=0);
    virtual ~QmlState();

    QString name() const;
    void setName(const QString &);

    /*'when' is a QmlBinding to limit state changes oscillation
     due to the unpredictable order of evaluation of bound expressions*/
    bool isWhenKnown() const;
    QmlBinding *when() const;
    void setWhen(QmlBinding *);

    QString extends() const;
    void setExtends(const QString &);

    QmlListProperty<QmlStateOperation> changes();
    int operationCount() const;
    QmlStateOperation *operationAt(int) const;

    QmlState &operator<<(QmlStateOperation *);

    void apply(QmlStateGroup *, QmlTransition *, QmlState *revert);
    void cancel();

    QmlStateGroup *stateGroup() const;
    void setStateGroup(QmlStateGroup *);

Q_SIGNALS:
    void completed();

private:
    Q_DECLARE_PRIVATE(QmlState)
    Q_DISABLE_COPY(QmlState)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlStateOperation)
QML_DECLARE_TYPE(QmlState)

QT_END_HEADER

#endif // QMLSTATE_H
