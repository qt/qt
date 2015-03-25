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

#include "qgraphicssystem_vg_p.h"
#include <QtOpenVG/private/qpixmapdata_vg_p.h>
#include <QtOpenVG/private/qwindowsurface_vg_p.h>
#include <QtOpenVG/private/qvgimagepool_p.h>
#if defined(Q_OS_SYMBIAN)
#include <QtGui/private/qwidget_p.h>
#endif
#include <QtGui/private/qapplication_p.h>

QT_BEGIN_NAMESPACE

QVGGraphicsSystem::QVGGraphicsSystem()
{
    QApplicationPrivate::graphics_system_name = QLatin1String("openvg");
}

QPixmapData *QVGGraphicsSystem::createPixmapData(QPixmapData::PixelType type) const
{
#if !defined(QVG_NO_SINGLE_CONTEXT) && !defined(QVG_NO_PIXMAP_DATA)
    // Pixmaps can use QVGPixmapData; bitmaps must use raster.
    if (type == QPixmapData::PixmapType)
        return new QVGPixmapData(type);
    else
        return new QRasterPixmapData(type);
#else
    return new QRasterPixmapData(type);
#endif
}

QWindowSurface *QVGGraphicsSystem::createWindowSurface(QWidget *widget) const
{
#if defined(Q_OS_SYMBIAN)
    if (!QApplicationPrivate::instance()->useTranslucentEGLSurfaces) {
        QWidgetPrivate *d = qt_widget_private(widget);
        if (!d->isOpaque && widget->testAttribute(Qt::WA_TranslucentBackground))
            return d->createDefaultWindowSurface_sys();
    }
#endif
    return new QVGWindowSurface(widget);
}

#ifdef Q_OS_SYMBIAN
void QVGGraphicsSystem::releaseCachedGpuResources()
{
    QVGImagePool::instance()->hibernate();
}

QGraphicsSystemEx *QVGGraphicsSystem::platformExtension()
{
    return this;
}
#endif
QT_END_NAMESPACE
