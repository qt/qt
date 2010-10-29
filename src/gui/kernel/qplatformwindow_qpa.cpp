/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qplatformwindow_qpa.h"

#include <QtGui/qwidget.h>

class QPlatformWindowPrivate
{
    QWidget *tlw;
    QRect rect;
    Qt::WindowFlags flags;
    friend class QPlatformWindow;
};

QPlatformWindow::QPlatformWindow(QWidget *tlw)
    : d_ptr(new QPlatformWindowPrivate)
{
    Q_D(QPlatformWindow);
    d->tlw = tlw;
    tlw->setPlatformWindow(this);
}

QPlatformWindow::~QPlatformWindow()
{
}

QWidget *QPlatformWindow::widget() const
{
    Q_D(const QPlatformWindow);
    return d->tlw;
}

void QPlatformWindow::setGeometry(const QRect &rect)
{
    Q_D(QPlatformWindow);
    d->rect = rect;
}

QRect QPlatformWindow::geometry() const
{
    Q_D(const QPlatformWindow);
    return d->rect;
}

/*!
Reimplemented in subclasses to show the surface if \a visible is \c true, and hide it if \a visible is \c false.
*/
void QPlatformWindow::setVisible(bool visible)
{
    Q_UNUSED(visible);
}
/*!
Requests setting the window flags of this surface to \a type. Returns the actual flags set.
*/
Qt::WindowFlags QPlatformWindow::setWindowFlags(Qt::WindowFlags flags)
{
    Q_D(QPlatformWindow);
    d->flags = flags;
    return flags;
}

/*!
  Returns the effective window flags for this surface.
*/
Qt::WindowFlags QPlatformWindow::windowFlags() const
{
    Q_D(const QPlatformWindow);
    return d->flags;
}

WId QPlatformWindow::winId() const { return WId(0); }

void QPlatformWindow::setParent(const QPlatformWindow *) { qWarning("This plugin does not support setParent!"); }

void QPlatformWindow::setWindowTitle(const QString &) {}

void QPlatformWindow::raise() { qWarning("This plugin does not support raise()"); }

void QPlatformWindow::lower() { qWarning("This plugin does not support lower()"); }

void QPlatformWindow::setOpacity(qreal level)
{
    Q_UNUSED(level);
    qWarning("This plugin does not support setting window opacity");
}

QPlatformGLContext *QPlatformWindow::glContext() const
{
    return 0;
}
