/****************************************************************************
**
** Copyright (C) 2012 Research In Motion
**
** Contact: Research In Motion <blackberry-qt@qnx.com>
** Contact: Klarälvdalens Datakonsult AB <info@kdab.com>
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

//#define QBBNAVIGATOREVENTHANDLER_DEBUG

#include "qbbnavigatoreventhandler.h"

#include <QApplication>
#include <QDebug>
#include <QWidget>
#include <QWindowSystemInterface>

QT_BEGIN_NAMESPACE

QBBNavigatorEventHandler::QBBNavigatorEventHandler(QObject *parent)
    : QObject(parent)
{
}

bool QBBNavigatorEventHandler::handleOrientationCheck(int angle)
{
#if defined(QBBNAVIGATOREVENTHANDLER_DEBUG)
    qDebug() << Q_FUNC_INFO << "angle=" << angle;
#else
    Q_UNUSED(angle);
#endif

    // reply to navigator that (any) orientation is acceptable
    // TODO: check if top window flags prohibit orientation change
    return true;
}

void QBBNavigatorEventHandler::handleOrientationChange(int angle)
{
    // update screen geometry and reply to navigator that we're ready
#if defined(QBBNAVIGATOREVENTHANDLER_DEBUG)
    qDebug() << Q_FUNC_INFO << "angle=" << angle;
#endif

    emit rotationChanged(angle);
}

void QBBNavigatorEventHandler::handleSwipeDown()
{
    // simulate menu key press
#if defined(QBBNAVIGATOREVENTHANDLER_DEBUG)
    qDebug() << Q_FUNC_INFO;
#endif

    QWidget *w = QApplication::activeWindow();
    QWindowSystemInterface::handleKeyEvent(w, QEvent::KeyPress, Qt::Key_Menu, Qt::NoModifier);
    QWindowSystemInterface::handleKeyEvent(w, QEvent::KeyRelease, Qt::Key_Menu, Qt::NoModifier);
}

void QBBNavigatorEventHandler::handleExit()
{
    // shutdown everything
#if defined(QBBNAVIGATOREVENTHANDLER_DEBUG)
    qDebug() << Q_FUNC_INFO;
#endif

    QApplication::quit();
}

void QBBNavigatorEventHandler::handleWindowGroupActivated(const QByteArray &id)
{
#if defined(QBBNAVIGATOREVENTHANDLER_DEBUG)
    qDebug() << Q_FUNC_INFO << id;
#endif

    Q_EMIT windowGroupActivated(id);
}

void QBBNavigatorEventHandler::handleWindowGroupDeactivated(const QByteArray &id)
{
#if defined(QBBNAVIGATOREVENTHANDLER_DEBUG)
    qDebug() << Q_FUNC_INFO << id;
#endif

    Q_EMIT windowGroupDeactivated(id);
}

QT_END_NAMESPACE
