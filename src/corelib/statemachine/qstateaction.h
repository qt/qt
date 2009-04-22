/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QSTATEACTION_H
#define QSTATEACTION_H

#include <QtCore/qobject.h>

#include <QtCore/qvariant.h>
#include <QtCore/qlist.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QStateActionPrivate;
class Q_CORE_EXPORT QStateAction : public QObject
{
    Q_OBJECT
public:
    ~QStateAction();

protected:
    QStateAction(QObject *parent = 0);

    virtual void execute() = 0;

    bool event(QEvent *e);

protected:
#ifdef QT_STATEMACHINE_SOLUTION
    QStateActionPrivate *d_ptr;
#endif
    QStateAction(QStateActionPrivate &dd, QObject *parent);

private:
    Q_DISABLE_COPY(QStateAction)
    Q_DECLARE_PRIVATE(QStateAction)
};

class QStateInvokeMethodActionPrivate;
class Q_CORE_EXPORT QStateInvokeMethodAction : public QStateAction
{
    Q_OBJECT
    Q_PROPERTY(QObject* target READ targetObject WRITE setTargetObject)
    Q_PROPERTY(QByteArray methodName READ methodName WRITE setMethodName)
    Q_PROPERTY(QVariantList arguments READ arguments WRITE setArguments)
public:
    QStateInvokeMethodAction(QObject *target, const QByteArray &methodName,
                             QObject *parent = 0);
    QStateInvokeMethodAction(QObject *target, const QByteArray &methodName,
                             const QList<QVariant> &args, QObject *parent = 0);
    QStateInvokeMethodAction(QObject *parent = 0);
    ~QStateInvokeMethodAction();

    QObject *targetObject() const;
    void setTargetObject(QObject *target);

    QByteArray methodName() const;
    void setMethodName(const QByteArray &methodName);

    QVariantList arguments() const;
    void setArguments(const QVariantList &arguments);

protected:
    void execute();

    bool event(QEvent *e);

private:
    Q_DISABLE_COPY(QStateInvokeMethodAction)
    Q_DECLARE_PRIVATE(QStateInvokeMethodAction)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
