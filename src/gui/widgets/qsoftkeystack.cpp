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
#include "qevent.h"

#if !defined(Q_WS_S60)
#include "qtoolbar.h"
#endif

static bool isSame(const QSoftkeySet& a, const QSoftkeySet& b)
{
    bool isSame=true;
    if ( a.count() != b.count())
        return false;
    int index=0;
    while (index<a.count()) {
        if (a.at(index)->role() != b.at(index)->role())
            return false;
        if (a.at(index)->text().compare(b.at(index)->text())!=0)
            return false;
        index++;
    }
    return true;
}

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

void QSoftKeyStackPrivate::popandPush(QSoftKeyAction *softKey)
{
    QSoftkeySet oldSoftKeySet = softKeyStack.pop();
    QSoftkeySet newSoftKeySet;
    newSoftKeySet.append(newSoftKeySet);
    softKeyStack.push(newSoftKeySet);
    if( !isSame(oldSoftKeySet, newSoftKeySet))
        setNativeSoftKeys();
}

void QSoftKeyStackPrivate::popandPush(const QList<QSoftKeyAction*> &softkeys)
{
    QSoftkeySet oldSoftKeySet = softKeyStack.pop();
    QSoftkeySet newSoftKeySet(softkeys);
    softKeyStack.push(newSoftKeySet);
    if( !isSame(oldSoftKeySet, newSoftKeySet))
        setNativeSoftKeys();
}

const QSoftkeySet& QSoftKeyStackPrivate::top()
{
    return softKeyStack.top();
}

bool QSoftKeyStackPrivate::isEmpty()
{
    return softKeyStack.isEmpty();
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

void QSoftKeyStack::popandPush(QSoftKeyAction *softKey)
{
    Q_D(QSoftKeyStack);
    d->popandPush(softKey);
}

void QSoftKeyStack::popandPush(const QList<QSoftKeyAction*> &softkeys)
{
    Q_D(QSoftKeyStack);
    d->popandPush(softkeys);
}

const QSoftkeySet& QSoftKeyStack::top()
{
    Q_D(QSoftKeyStack);
    return d->top();
}

bool QSoftKeyStack::isEmpty()
{
    Q_D(QSoftKeyStack);
    return d->isEmpty();
}

void QSoftKeyStack::handleFocusChanged(QWidget *old, QWidget *now)
{
    if (!now)
        return;
    QWidget *w = qApp->activeWindow();
    QMainWindow *mainWindow = qobject_cast<QMainWindow*>(w);
    if( !mainWindow)
        return;
    QSoftKeyStack* softKeyStack = mainWindow->softKeyStack();
    
    Qt::ContextMenuPolicy policy = now->contextMenuPolicy();
    if (policy != Qt::NoContextMenu && policy != Qt::PreventContextMenu ) {
        QList<QSoftKeyAction*> actionList;
        QSoftKeyAction* menu = new QSoftKeyAction(QSoftKeyAction::Menu, QString::fromLatin1("Menu"), now);
        QSoftKeyAction* contextMenu = new QSoftKeyAction(QSoftKeyAction::ContextMenu, QString::fromLatin1("ContextMenu"), now);
        actionList.append(menu);
        actionList.append(contextMenu);
        if (old)
            softKeyStack->popandPush(actionList);
        else
            softKeyStack->push(actionList);
    }
}

void QSoftKeyStack::handleSoftKeyPress(int command)
{
    Q_D(QSoftKeyStack);
    d->handleSoftKeyPress(command);
}

QMainWindow *QSoftKeyStack::mainWindowOfWidget(QWidget *widget)
{
    QWidget *widgetParent = widget;
    while (widgetParent = widgetParent->parentWidget())
        if (QMainWindow *mainWindow = qobject_cast<QMainWindow*>(widgetParent))
            return mainWindow;

    return 0;
}

QSoftKeyStack *QSoftKeyStack::softKeyStackOfWidget(QWidget *widget)
{
    QMainWindow *mainWindow = mainWindowOfWidget(widget);
    return (mainWindow && mainWindow->hasSoftKeyStack()) ? mainWindow->softKeyStack() : 0;
}

#if !defined(Q_WS_S60)
void QSoftKeyStackPrivate::handleSoftKeyPress(int command)
{
    Q_UNUSED(command)
}

QToolBar* softKeyToolBar(QMainWindow *mainWindow)
{
    Q_ASSERT(mainWindow);
    const QString toolBarName = QString::fromLatin1("SoftKeyToolBarForDebugging");
    QToolBar *result = 0;
    foreach (QObject *child, mainWindow->children()) {
        result = qobject_cast<QToolBar*>(child);
        if (result && result->objectName() == toolBarName)
            return result;
    }
    result = mainWindow->addToolBar(toolBarName);
    result->setObjectName(toolBarName);
    return result;
}

void QSoftKeyStackPrivate::setNativeSoftKeys()
{
    Q_Q(QSoftKeyStack);
    QMainWindow *parent = qobject_cast<QMainWindow*>(q->parent());
    if (!parent)
        return;
    QToolBar* toolbar = softKeyToolBar(parent);
    toolbar->clear();
    foreach (const QSoftkeySet &set, softKeyStack) {
        foreach (QSoftKeyAction *skAction, set)
            toolbar->addAction(skAction);
        toolbar->addSeparator();
    }
    if (toolbar->actions().isEmpty()) {
        parent->removeToolBar(toolbar);
        delete toolbar;
    }
}
#endif // !defined(Q_WS_S60)

QKeyEventSoftKey::QKeyEventSoftKey(QSoftKeyAction *softKeyAction, Qt::Key key, QObject *parent)
    : QObject(parent)
    , m_softKeyAction(softKeyAction)
    , m_key(key)
{
}

void QKeyEventSoftKey::addSoftKey(QSoftKeyAction::StandardRole standardRole, Qt::Key key, QWidget *actionWidget)
{
    QSoftKeyStack *stack = QSoftKeyStack::softKeyStackOfWidget(actionWidget);
    if (!stack)
        return;
    QSoftKeyAction *action = new QSoftKeyAction(standardRole, actionWidget);
    QKeyEventSoftKey *softKey = new QKeyEventSoftKey(action, key, actionWidget);
    connect(action, SIGNAL(triggered()), softKey, SLOT(sendKeyEvent()));
    stack->push(action);
}

void QKeyEventSoftKey::sendKeyEvent()
{
    QApplication::postEvent(parent(), new QKeyEvent(QEvent::KeyPress, m_key, Qt::NoModifier));
}
