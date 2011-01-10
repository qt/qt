/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the config.tests of the Qt Toolkit.
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

#include "qwaylandwindow.h"

#include "qwaylanddisplay.h"
#include "qwaylandwindowsurface.h"
#include "qwaylandglcontext.h"

#include <QtGui/QWidget>
#include <QtGui/QWindowSystemInterface>

QWaylandWindow::QWaylandWindow(QWidget *window, QWaylandDisplay *display)
    : QPlatformWindow(window)
    , mSurface(0)
    , mDisplay(display)
    , mGLContext(0)
    , mBuffer(0)
{
    static WId id = 1;

    mWindowId = id++;
}

QWaylandWindow::~QWaylandWindow()
{
    if (mGLContext)
        delete mGLContext;
}

WId QWaylandWindow::winId() const
{
    return mWindowId;
}

void QWaylandWindow::setParent(const QPlatformWindow *parent)
{
    QWaylandWindow *wParent = (QWaylandWindow *)parent;

    mParentWindow = wParent;
}

void QWaylandWindow::setVisible(bool visible)
{
    if (visible) {
        mSurface = mDisplay->createSurface();
        wl_surface_set_user_data(mSurface, this);
        attach(mBuffer);
    } else {
        wl_surface_destroy(mSurface);
        mSurface = NULL;
    }
}

void QWaylandWindow::attach(QWaylandBuffer *buffer)
{
    QRect geometry = widget()->geometry();

    mBuffer = buffer;
    if (mSurface) {
        wl_surface_attach(mSurface, mBuffer->mBuffer,geometry.x(),geometry.y());
        wl_surface_map_toplevel(mSurface);
    }
}

void QWaylandWindow::configure(uint32_t time, uint32_t edges,
                               int32_t x, int32_t y,
                               int32_t width, int32_t height)
{
    Q_UNUSED(time);
    Q_UNUSED(edges);
    QRect geometry = QRect(x, y, width, height);

    QWindowSystemInterface::handleGeometryChange(widget(), geometry);
}
