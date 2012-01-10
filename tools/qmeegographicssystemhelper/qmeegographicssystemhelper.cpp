/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#define ENSURE_RUNNING_MEEGO {if (! QMeeGoGraphicsSystemHelper::isRunningMeeGo()) { qFatal("Using meego functionality but not running meego graphics system!"); }}

#include "qmeegographicssystemhelper.h"
#include <private/qapplication_p.h>
#include <private/qgraphicssystem_runtime_p.h>
#include <private/qpixmap_raster_p.h>
#include <private/qwindowsurface_gl_p.h>
#include "qmeegoruntime.h"

QString QMeeGoGraphicsSystemHelper::runningGraphicsSystemName()
{
    if (! QApplicationPrivate::instance()) {
        qWarning("Querying graphics system but application not running yet!");
        return QString();
    }

    QString name = QApplicationPrivate::instance()->graphics_system_name;
    if (name == QLatin1String("runtime")) {
        QRuntimeGraphicsSystem *rsystem = (QRuntimeGraphicsSystem *) QApplicationPrivate::instance()->graphics_system;
        name = rsystem->graphicsSystemName();
    }

    return name;
}

bool QMeeGoGraphicsSystemHelper::isRunningMeeGo()
{
    return (runningGraphicsSystemName() == QLatin1String("meego"));
}

bool QMeeGoGraphicsSystemHelper::isRunningRuntime()
{
    return (QApplicationPrivate::instance()->graphics_system_name == QLatin1String("runtime"));
}

void QMeeGoGraphicsSystemHelper::switchToMeeGo()
{
    QMeeGoRuntime::switchToMeeGo();
}

void QMeeGoGraphicsSystemHelper::switchToRaster()
{
    QMeeGoRuntime::switchToRaster();
}

Qt::HANDLE QMeeGoGraphicsSystemHelper::imageToEGLSharedImage(const QImage &image)
{
    ENSURE_RUNNING_MEEGO;
    return QMeeGoRuntime::imageToEGLSharedImage(image);
}

QPixmap QMeeGoGraphicsSystemHelper::pixmapFromEGLSharedImage(Qt::HANDLE handle, const QImage &softImage)
{
    // This function is supported when not running meego too. A raster-backed
    // pixmap will be created... but when you switch back to 'meego', it'll 
    // be replaced with a EGL shared image backing.
    return QPixmap(QMeeGoRuntime::pixmapDataFromEGLSharedImage(handle, softImage));
}

QPixmap QMeeGoGraphicsSystemHelper::pixmapWithGLTexture(int w, int h)
{
    ENSURE_RUNNING_MEEGO;
    return QPixmap(QMeeGoRuntime::pixmapDataWithGLTexture(w, h));
}

bool QMeeGoGraphicsSystemHelper::destroyEGLSharedImage(Qt::HANDLE handle)
{
    ENSURE_RUNNING_MEEGO;
    return QMeeGoRuntime::destroyEGLSharedImage(handle);
}

void QMeeGoGraphicsSystemHelper::updateEGLSharedImagePixmap(QPixmap *p)
{
    ENSURE_RUNNING_MEEGO;
    return QMeeGoRuntime::updateEGLSharedImagePixmap(p);
}

void QMeeGoGraphicsSystemHelper::setTranslucent(bool translucent)
{
    ENSURE_RUNNING_MEEGO;
    QMeeGoRuntime::setTranslucent(translucent);
}

void QMeeGoGraphicsSystemHelper::setSwapBehavior(SwapMode mode)
{
    ENSURE_RUNNING_MEEGO;

    if (mode == AutomaticSwap)
        QGLWindowSurface::swapBehavior = QGLWindowSurface::AutomaticSwap;
    else if (mode == AlwaysFullSwap)
        QGLWindowSurface::swapBehavior = QGLWindowSurface::AlwaysFullSwap;
    else if (mode == AlwaysPartialSwap)
        QGLWindowSurface::swapBehavior = QGLWindowSurface::AlwaysPartialSwap;
    else if (mode == KillSwap)
        QGLWindowSurface::swapBehavior = QGLWindowSurface::KillSwap;
}

void QMeeGoGraphicsSystemHelper::setSwitchPolicy(SwitchPolicy policy)
{
    QMeeGoRuntime::setSwitchPolicy(policy);
}

void QMeeGoGraphicsSystemHelper::enableSwitchEvents()
{
    QMeeGoRuntime::enableSwitchEvents();
}
