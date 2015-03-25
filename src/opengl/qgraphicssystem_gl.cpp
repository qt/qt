/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#include "qgraphicssystem_gl_p.h"
#include <QGraphicsView>

#include "private/qpixmap_raster_p.h"
#include "private/qpixmapdata_gl_p.h"
#include "private/qwindowsurface_gl_p.h"
#include "private/qgl_p.h"
#include <private/qwindowsurface_raster_p.h>

#if defined(Q_WS_X11) && !defined(QT_NO_EGL)
#include "private/qpixmapdata_x11gl_p.h"
#include "private/qwindowsurface_x11gl_p.h"
#endif

#if defined(Q_OS_SYMBIAN)
#include <QtGui/private/qapplication_p.h>
#include "private/qgltexturepool_p.h"
#endif

QT_BEGIN_NAMESPACE

extern QGLWidget *qt_gl_getShareWidget();

QPixmapData *QGLGraphicsSystem::createPixmapData(QPixmapData::PixelType type) const
{
    return new QGLPixmapData(type);
}

QWindowSurface *QGLGraphicsSystem::createWindowSurface(QWidget *widget) const
{
#ifdef Q_WS_WIN
    // On Windows the QGLWindowSurface class can't handle
    // drop shadows and native effects, e.g. fading a menu in/out using
    // top level window opacity.
    if (widget->windowType() == Qt::Popup)
        return new QRasterWindowSurface(widget);
#endif

#if defined(Q_WS_X11) && !defined(QT_NO_EGL)
    if (m_useX11GL && QX11GLPixmapData::hasX11GLPixmaps()) {
        // If the widget is a QGraphicsView which will be re-drawing the entire
        // scene each frame anyway, we should use QGLWindowSurface as this may
        // provide proper buffer flipping, which should be faster than QX11GL's
        // blitting approach:
        QGraphicsView* qgv = qobject_cast<QGraphicsView*>(widget);
        if (qgv && qgv->viewportUpdateMode() == QGraphicsView::FullViewportUpdate)
            return new QGLWindowSurface(widget);
        else
            return new QX11GLWindowSurface(widget);
    }
#endif

#if defined(Q_OS_SYMBIAN)
    if (!QApplicationPrivate::instance()->useTranslucentEGLSurfaces) {
        QWidgetPrivate *d = qt_widget_private(widget);
        if (!d->isOpaque && widget->testAttribute(Qt::WA_TranslucentBackground))
            return d->createDefaultWindowSurface_sys();
    }
#endif

    return new QGLWindowSurface(widget);
}
#ifdef Q_OS_SYMBIAN
void QGLGraphicsSystem::releaseCachedGpuResources()
{
    QGLTexturePool::instance()->hibernate();
}

QGraphicsSystemEx* QGLGraphicsSystem::platformExtension()
{
    return this;
}
#endif

QT_END_NAMESPACE

