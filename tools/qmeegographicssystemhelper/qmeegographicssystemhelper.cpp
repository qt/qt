/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#define ENSURE_RUNNING_MEEGO {if (! QMeeGoGraphicsSystemHelper::isRunningMeeGo()) { qFatal("Using meego functionality but not running meego graphics system!"); }}

#include "qmeegographicssystemhelper.h"
#include <private/qapplication_p.h>
#include <private/qgraphicssystem_runtime_p.h>
#include <private/qpixmap_raster_p.h>
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
    if (isRunningMeeGo())
        return;

    if (QApplicationPrivate::instance()->graphics_system_name != QLatin1String("runtime"))
        qWarning("Can't switch to meego - switching only supported with 'runtime' graphics system.");
    else {
        QApplication *app = static_cast<QApplication *>(QCoreApplication::instance());
        app->setGraphicsSystem(QLatin1String("meego"));
    }
}

void QMeeGoGraphicsSystemHelper::switchToRaster()
{
    if (runningGraphicsSystemName() == QLatin1String("raster"))
        return;

    if (QApplicationPrivate::instance()->graphics_system_name != QLatin1String("runtime"))
        qWarning("Can't switch to raster - switching only supported with 'runtime' graphics system.");
    else {
        QApplication *app = static_cast<QApplication *>(QCoreApplication::instance());
        app->setGraphicsSystem(QLatin1String("raster"));
    }
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
