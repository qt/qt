/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the config.tests of the Qt Toolkit.
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

#include "qwaylandwindow.h"

#include "qwaylandbuffer.h"
#include "qwaylanddisplay.h"
#include "qwaylandinputdevice.h"
#include "qwaylandscreen.h"

#ifdef QT_WAYLAND_WINDOWMANAGER_SUPPORT
#include "windowmanager_integration/qwaylandwindowmanagerintegration.h"
#endif

#include <QCoreApplication>
#include <QtGui/QWidget>
#include <QtGui/QWindowSystemInterface>

#include <QDebug>

QWaylandWindow::QWaylandWindow(QWidget *window)
    : QPlatformWindow(window)
    , mSurface(0)
    , mDisplay(QWaylandScreen::waylandScreenFromWidget(window)->display())
    , mBuffer(0)
    , mWaitingForFrameSync(false)
{
    static WId id = 1;
    mWindowId = id++;

#ifdef QT_WAYLAND_WINDOWMANAGER_SUPPORT
        mDisplay->windowManagerIntegration()->mapClientToProcess(qApp->applicationPid());
        mDisplay->windowManagerIntegration()->authenticateWithToken();
#endif
}

QWaylandWindow::~QWaylandWindow()
{
    if (mSurface)
        wl_surface_destroy(mSurface);

    QList<QWaylandInputDevice *> inputDevices = mDisplay->inputDevices();
    for (int i = 0; i < inputDevices.size(); ++i)
        inputDevices.at(i)->handleWindowDestroyed(this);
}

WId QWaylandWindow::winId() const
{
    return mWindowId;
}

void QWaylandWindow::setParent(const QPlatformWindow *parent)
{
    Q_UNUSED(parent);
    qWarning("Sub window is not supported");
}

void QWaylandWindow::setVisible(bool visible)
{
    if (!mSurface && visible) {
        mSurface = mDisplay->createSurface(this);
        newSurfaceCreated();
    }

    if (!visible) {
        wl_surface_destroy(mSurface);
        mSurface = NULL;
    }
}

void QWaylandWindow::configure(uint32_t time, uint32_t edges,
                               int32_t x, int32_t y,
                               int32_t width, int32_t height)
{
    Q_UNUSED(time);
    Q_UNUSED(edges);
    QRect geometry = QRect(x, y, width, height);

    setGeometry(geometry);

    QWindowSystemInterface::handleGeometryChange(widget(), geometry);
}

void QWaylandWindow::attach(QWaylandBuffer *buffer)
{
    mBuffer = buffer;
    if (mSurface) {
        wl_surface_attach(mSurface, buffer->buffer(),0,0);
    }
}

void QWaylandWindow::damage(const QRect &rect)
{
    //We have to do sync stuff before calling damage, or we might
    //get a frame callback before we get the timestamp
    if (!mWaitingForFrameSync) {
        mDisplay->frameCallback(QWaylandWindow::frameCallback, mSurface, this);
        mWaitingForFrameSync = true;
    }

    wl_surface_damage(mSurface,
                      rect.x(), rect.y(), rect.width(), rect.height());
}

void QWaylandWindow::newSurfaceCreated()
{
    if (mBuffer) {
        wl_surface_attach(mSurface,mBuffer->buffer(),0,0);
        wl_surface_damage(mSurface,
                          0,0,mBuffer->size().width(),mBuffer->size().height());
    }
}

void QWaylandWindow::frameCallback(struct wl_surface *surface, void *data, uint32_t time)
{
    Q_UNUSED(time);
    Q_UNUSED(surface);
    QWaylandWindow *self = static_cast<QWaylandWindow*>(data);
    self->mWaitingForFrameSync = false;
}

void QWaylandWindow::waitForFrameSync()
{
    mDisplay->flushRequests();
    while (mWaitingForFrameSync)
        mDisplay->blockingReadEvents();
}
