/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include "qgraphicspiemenu.h"
#include "qgraphicspiemenu_p.h"

#include <QtGui/qaction.h>

QGraphicsPieMenu::QGraphicsPieMenu(QGraphicsItem *parent)
    : QGraphicsWidget(parent), d_ptr(new QGraphicsPieMenuPrivate)
{
    d_ptr->q_ptr = this;
    d_ptr->machine = new QStateMachine();
    d_ptr->popupState = new QState(d_ptr->machine->rootState());
    d_ptr->machine->setInitialState(d_ptr->popupState);
    d_ptr->menuAction = new QAction(this);
}

QGraphicsPieMenu::QGraphicsPieMenu(const QString &title, QGraphicsItem *parent)
    : QGraphicsWidget(parent), d_ptr(new QGraphicsPieMenuPrivate)
{
    d_ptr->q_ptr = this;
    d_ptr->machine = new QStateMachine();
    d_ptr->popupState = new QState(d_ptr->machine->rootState());
    d_ptr->machine->setInitialState(d_ptr->popupState);
    d_ptr->menuAction = new QAction(this);
    setTitle(title);
}

QGraphicsPieMenu::QGraphicsPieMenu(const QIcon &icon, const QString &title, QGraphicsItem *parent)
    : QGraphicsWidget(parent), d_ptr(new QGraphicsPieMenuPrivate)
{
    d_ptr->q_ptr = this;
    d_ptr->machine = new QStateMachine();
    d_ptr->popupState = new QState(d_ptr->machine->rootState());
    d_ptr->machine->setInitialState(d_ptr->popupState);
    d_ptr->menuAction = new QAction(this);
    setIcon(icon);
    setTitle(title);
}

QGraphicsPieMenu::~QGraphicsPieMenu()
{
    delete d_ptr;
}

QAction *QGraphicsPieMenu::addAction(const QString &text)
{
    QAction *action = new QAction(text, this);
    addAction(action);
    return action;
}

QAction *QGraphicsPieMenu::addAction(const QIcon &icon, const QString &text)
{
    QAction *action = new QAction(icon, text, this);
    addAction(action);
    return action;
}

QAction *QGraphicsPieMenu::addAction(const QString &text, const QObject *receiver, const char *member, const QKeySequence &shortcut)
{
    QAction *action = new QAction(text, this);
    action->setShortcut(shortcut);
    connect(action, SIGNAL(triggered(bool)), receiver, member);
    addAction(action);
    return action;
}

QAction *QGraphicsPieMenu::addAction(const QIcon &icon, const QString &text, const QObject *receiver, const char *member, const QKeySequence &shortcut)
{
    QAction *action = new QAction(icon, text, this);
    action->setShortcut(shortcut);
    connect(action, SIGNAL(triggered(bool)), receiver, member);
    addAction(action);
    return action;
}

QAction *QGraphicsPieMenu::addMenu(QGraphicsPieMenu *menu)
{
    QAction *action = menu->menuAction();
    addAction(action);
    return action;
}

QGraphicsPieMenu *QGraphicsPieMenu::addMenu(const QString &title)
{
    QGraphicsPieMenu *menu = new QGraphicsPieMenu(title, this);
    addMenu(menu);
    return menu;
}

QGraphicsPieMenu *QGraphicsPieMenu::addMenu(const QIcon &icon, const QString &title)
{
    QGraphicsPieMenu *menu = new QGraphicsPieMenu(icon, title, this);
    addMenu(menu);
    return menu;
}

QAction *QGraphicsPieMenu::addSeparator()
{
    QAction *action = new QAction(this);
    action->setSeparator(true);
    addAction(action);
    return action;
}

QAction *QGraphicsPieMenu::insertMenu(QAction *before, QGraphicsPieMenu *menu)
{
    QAction *action = menu->menuAction();
    insertAction(before, action);
    return action;
}

QAction *QGraphicsPieMenu::insertSeparator(QAction *before)
{
    QAction *action = new QAction(this);
    action->setSeparator(true);
    insertAction(before, action);
    return action;
}

QAction *QGraphicsPieMenu::menuAction() const
{
    return d_func()->menuAction;
}

bool QGraphicsPieMenu::isEmpty() const
{
    // ### d->actions
    QList<QAction *> actionList = actions();
    bool ret = true;
    for (int i = 0; ret && i < actionList.size(); ++i) {
        const QAction *action = actionList.at(i);
        if (!action->isSeparator() && action->isVisible()) {
            ret = false;
            break;
        }
    }
    return ret;
}

void QGraphicsPieMenu::clear()
{
    // ### d->actions
    QList<QAction *> actionList = actions();
    for(int i = 0; i < actionList.size(); i++) {
        QAction *action = actionList.at(i);
        removeAction(action);
        if (action->parent() == this && action->associatedGraphicsWidgets().isEmpty())
            delete action;
    }
}

void QGraphicsPieMenu::popup(const QPointF &pos)
{
    Q_UNUSED(pos);
    Q_D(QGraphicsPieMenu);
    d->machine->start();
}

QAction *QGraphicsPieMenu::exec()
{
    return exec(pos());
}

QAction *QGraphicsPieMenu::exec(const QPointF &pos)
{
    Q_UNUSED(pos);
    return 0;
}

QAction *QGraphicsPieMenu::exec(QList<QAction *> actions, const QPointF &pos)
{
    QGraphicsPieMenu menu;
    for (QList<QAction *>::ConstIterator it = actions.constBegin(); it != actions.constEnd(); ++it)
        menu.addAction(*it);
    return menu.exec(pos);
}

QString QGraphicsPieMenu::title() const
{
    Q_D(const QGraphicsPieMenu);
    return d->title;
}

void QGraphicsPieMenu::setTitle(const QString &title)
{
    Q_D(QGraphicsPieMenu);
    d->title = title;
    updateGeometry();
}

QIcon QGraphicsPieMenu::icon() const
{
    Q_D(const QGraphicsPieMenu);
    return d->icon;
}

void QGraphicsPieMenu::setIcon(const QIcon &icon)
{
    Q_D(QGraphicsPieMenu);
    d->icon = icon;
    updateGeometry();
}

QSizeF QGraphicsPieMenu::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    Q_UNUSED(which);
    Q_UNUSED(constraint);
    return QSizeF(1, 1);
}
