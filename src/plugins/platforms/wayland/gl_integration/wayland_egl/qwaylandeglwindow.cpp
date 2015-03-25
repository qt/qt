/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qwaylandeglwindow.h"

#include "qwaylandscreen.h"
#include "qwaylandglcontext.h"

QWaylandEglWindow::QWaylandEglWindow(QWidget *window)
    : QWaylandWindow(window)
    , mGLContext(0)
    , mWaylandEglWindow(0)
{
    mEglIntegration = static_cast<QWaylandEglIntegration *>(mDisplay->eglIntegration());
    //super creates a new surface
    newSurfaceCreated();
}

QWaylandEglWindow::~QWaylandEglWindow()
{
    delete mGLContext;
}

QWaylandWindow::WindowType QWaylandEglWindow::windowType() const
{
    return QWaylandWindow::Egl;
}

void QWaylandEglWindow::setGeometry(const QRect &rect)
{
    QWaylandWindow::setGeometry(rect);
    if (mWaylandEglWindow) {
        wl_egl_window_resize(mWaylandEglWindow,rect.width(),rect.height(),0,0);
    }
}

void QWaylandEglWindow::setParent(const QPlatformWindow *parent)
{
    const QWaylandWindow *wParent = static_cast<const QWaylandWindow *>(parent);

    mParentWindow = wParent;
}

QPlatformGLContext * QWaylandEglWindow::glContext() const
{
    if (!mGLContext) {
        QWaylandEglWindow *that = const_cast<QWaylandEglWindow *>(this);
        that->mGLContext = new QWaylandGLContext(mEglIntegration->eglDisplay(),widget()->platformWindowFormat());

        EGLNativeWindowType window(reinterpret_cast<EGLNativeWindowType>(mWaylandEglWindow));
        EGLSurface surface = eglCreateWindowSurface(mEglIntegration->eglDisplay(),mGLContext->eglConfig(),window,NULL);
        that->mGLContext->setEglSurface(surface);
    }

    return mGLContext;
}

void QWaylandEglWindow::newSurfaceCreated()
{
    if (mWaylandEglWindow) {
        wl_egl_window_destroy(mWaylandEglWindow);
    }
    wl_visual *visual = QWaylandScreen::waylandScreenFromWidget(widget())->visual();
    QSize size = geometry().size();
    if (!size.isValid())
        size = QSize(0,0);

    mWaylandEglWindow = wl_egl_window_create(mSurface,size.width(),size.height(),visual);
    if (mGLContext) {
        EGLNativeWindowType window(reinterpret_cast<EGLNativeWindowType>(mWaylandEglWindow));
        EGLSurface surface = eglCreateWindowSurface(mEglIntegration->eglDisplay(),mGLContext->eglConfig(),window,NULL);
        mGLContext->setEglSurface(surface);
    }
}
