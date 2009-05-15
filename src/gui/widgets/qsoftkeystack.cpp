/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qsoftkeystack.h"
#include "qsoftkeystack_p.h"
#include "qapplication.h"
#include "qmainwindow.h"

QSoftKeyStackPrivate::QSoftKeyStackPrivate()
{

}

QSoftKeyStackPrivate::~QSoftKeyStackPrivate()
{

}

void QSoftKeyStackPrivate::push(QSoftKeyAction *softKey)
{
    QSoftkeySet softKeySet;
    softKeySet.append(softKey);
    softKeyStack.push(softKeySet);
    setNativeSoftKeys();

}
void QSoftKeyStackPrivate::push(const QList<QSoftKeyAction*> &softkeys)
{
    QSoftkeySet softKeySet(softkeys);
    softKeyStack.push(softKeySet);
    setNativeSoftKeys();
}

void QSoftKeyStackPrivate::pop()
{
    softKeyStack.pop();
    setNativeSoftKeys();
}

const QSoftkeySet& QSoftKeyStackPrivate::top()
{
    return softKeyStack.top();
}

QSoftKeyStack::QSoftKeyStack(QWidget *parent)
    : QObject(*new QSoftKeyStackPrivate, parent)
{
    connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), SLOT(handleFocusChanged(QWidget*, QWidget*)));
}

QSoftKeyStack::~QSoftKeyStack()
{
}

void QSoftKeyStack::push(QSoftKeyAction *softKey)
{
    Q_D(QSoftKeyStack);
    d->push(softKey);
}

void QSoftKeyStack::push(const QList<QSoftKeyAction*> &softKeys)
{
    Q_D(QSoftKeyStack);
    d->push(softKeys);
}

void QSoftKeyStack::pop()
{
    Q_D(QSoftKeyStack);
    d->pop();
}

const QSoftkeySet& QSoftKeyStack::top()
{
    Q_D(QSoftKeyStack);
    return d->top();
}

void QSoftKeyStack::handleFocusChanged(QWidget *old, QWidget *now)
{
    if (!now)
        return;
    bool nowInOurMainWindow = false;
    const QMainWindow *ourMainWindow = qobject_cast<const QMainWindow*>(parent());
    Q_ASSERT(ourMainWindow);

    // "ourMainWindow" in parent chain of "now"? Isn't there a helper in Qt for this?
    QWidget *nowParent = now;
    while (nowParent = nowParent->parentWidget()) {
        if (nowParent == ourMainWindow) {
            nowInOurMainWindow = true;
            break;
        }
    }

    if (!nowInOurMainWindow)
        return;

    QList<QAction*> actions = now->actions();
    // Do something with these actions.
}
