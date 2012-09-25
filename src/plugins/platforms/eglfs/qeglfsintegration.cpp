/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qeglfsintegration.h"

#include "qeglfswindow.h"
#include "qeglfswindowsurface.h"

#include "qgenericunixfontdatabase.h"

#include <QtGui/QPlatformWindow>
#include <QtGui/QPlatformWindowFormat>
#include <QtOpenGL/private/qpixmapdata_gl_p.h>

#include <EGL/egl.h>

QT_BEGIN_NAMESPACE

QEglFSIntegration::QEglFSIntegration()
    : mFontDb(new QGenericUnixFontDatabase())
{
    m_primaryScreen = new QEglFSScreen(EGL_DEFAULT_DISPLAY);

    mScreens.append(m_primaryScreen);
#ifdef QEGL_EXTRA_DEBUG
    qWarning("QEglIntegration\n");
#endif
}

bool QEglFSIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
    switch (cap) {
    case ThreadedPixmaps: return true;
    default: return QPlatformIntegration::hasCapability(cap);
    }
}

QPixmapData *QEglFSIntegration::createPixmapData(QPixmapData::PixelType type) const
{
#ifdef QEGL_EXTRA_DEBUG
    qWarning("QEglIntegration::createPixmapData %d\n", type);
#endif
    return new QGLPixmapData(type);
}

QPlatformWindow *QEglFSIntegration::createPlatformWindow(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
#ifdef QEGL_EXTRA_DEBUG
    qWarning("QEglIntegration::createPlatformWindow %p\n",widget);
#endif
    return new QEglFSWindow(widget, m_primaryScreen);
}


QWindowSurface *QEglFSIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);

#ifdef QEGL_EXTRA_DEBUG
    qWarning("QEglIntegration::createWindowSurface %p\n",widget);
#endif
    return new QEglFSWindowSurface(m_primaryScreen,widget);
}

QPlatformFontDatabase *QEglFSIntegration::fontDatabase() const
{
    return mFontDb;
}

QT_END_NAMESPACE
