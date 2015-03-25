/****************************************************************************
**
** Copyright (C) 2012 Research In Motion <blackberry-qt@qnx.com>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
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
#if defined(QBBNAVIGATOREVENTHANDLER_DEBUG)
    qDebug() << Q_FUNC_INFO;
#endif
    QWidget *w = QApplication::activeWindow();
    if (w)
        QWindowSystemInterface::handlePlatformPanelEvent(w);
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

void QBBNavigatorEventHandler::handleWindowGroupStateChanged(const QByteArray &id, Qt::WindowState state)
{
#if defined(QBBNAVIGATOREVENTHANDLER_DEBUG)
    qDebug() << Q_FUNC_INFO << id;
#endif

    Q_EMIT windowGroupStateChanged(id, state);
}

QT_END_NAMESPACE
