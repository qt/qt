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

#ifndef QMLSTATE_H
#define QMLSTATE_H

#include <QtCore/qobject.h>
#include <QtCore/QSequentialAnimationGroup>
#include <QtDeclarative/qfxglobal.h>
#include <QtDeclarative/qml.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class ActionEvent;
class QmlBindableValue;
class Action
{
public:
    Action();

    bool restore:1;
    bool actionDone:1;

    QmlMetaProperty property;
    QVariant fromValue;
    QVariant toValue;

    QmlBindableValue *fromBinding;
    QmlBindableValue *toBinding;
    ActionEvent *event;

    void deleteFromBinding();
};

class ActionEvent 
{
public:
    virtual ~ActionEvent();
    virtual QString name() const;
    virtual void execute();
};

class QmlStateGroup;
class Q_DECLARATIVE_EXPORT QmlStateOperation : public QObject
{
    Q_OBJECT
public:
    QmlStateOperation(QObject *parent = 0)
        : QObject(parent) {}
    typedef QList<Action> ActionList;

    virtual ActionList actions();

protected:
    QmlStateOperation(QObjectPrivate &dd, QObject *parent = 0);
};
QML_DECLARE_TYPE(QmlStateOperation)

typedef QmlStateOperation::ActionList QmlStateActions;

class QmlTransition;
class QmlTransitionPrivate;
class QmlStatePrivate;
class Q_DECLARATIVE_EXPORT QmlState : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QmlBindableValue *when READ when WRITE setWhen)
    Q_PROPERTY(QString extends READ extends WRITE setExtends)
    Q_PROPERTY(QmlList<QmlStateOperation *>* operations READ operations)
    Q_CLASSINFO("DefaultProperty", "operations")

public:
    QmlState(QObject *parent=0);
    virtual ~QmlState();

    QString name() const;
    void setName(const QString &);

    /*'when' is a QmlBindableValue to limit state changes oscillation 
     due to the unpredictable order of evaluation of bound expressions*/
    bool isWhenKnown() const;
    QmlBindableValue *when() const;
    void setWhen(QmlBindableValue *);

    QString extends() const;
    void setExtends(const QString &);

    QmlList<QmlStateOperation *> *operations();
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
    friend class QmlTransitionPrivate;
};
QML_DECLARE_TYPE(QmlState)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLSTATE_H
