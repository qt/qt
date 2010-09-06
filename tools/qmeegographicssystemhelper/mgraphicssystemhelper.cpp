/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#define ENSURE_RUNNING_MEEGO {if (! MGraphicsSystemHelper::isRunningMeeGo()) { qFatal("Using meego functionality but not running meego graphics system!"); }}

#include "mgraphicssystemhelper.h"
#include "../private/qapplication_p.h"
#include "../private/qgraphicssystem_runtime_p.h"
#include "../private/qpixmap_raster_p.h"
#include "mruntime.h"

QString MGraphicsSystemHelper::runningGraphicsSystemName()
{
    if (! QApplicationPrivate::instance()) {
        qWarning("Querying graphics system but application not running yet!");
        return "";
    }

    QString name = QApplicationPrivate::instance()->graphics_system_name;
    if (name == "runtime") {
        QRuntimeGraphicsSystem *rsystem = (QRuntimeGraphicsSystem *) QApplicationPrivate::instance()->graphics_system;
        name = rsystem->graphicsSystemName();
    }

    return name;
}

bool MGraphicsSystemHelper::isRunningMeeGo()
{
    return (runningGraphicsSystemName() == "meego");
}

void MGraphicsSystemHelper::switchToMeeGo()
{
    if (runningGraphicsSystemName() == "meego")
        return;

    if (QApplicationPrivate::instance()->graphics_system_name != "runtime")
        qWarning("Can't switch to meego - switching only supported with 'runtime' graphics system.");
    else {
        QApplication *app = static_cast<QApplication *>(QCoreApplication::instance());
        app->setGraphicsSystem("meego");
    }
}

void MGraphicsSystemHelper::switchToRaster()
{
    if (runningGraphicsSystemName() == "raster")
        return;

    if (QApplicationPrivate::instance()->graphics_system_name != "runtime")
        qWarning("Can't switch to raster - switching only supported with 'runtime' graphics system.");
    else {
        QApplication *app = static_cast<QApplication *>(QCoreApplication::instance());
        app->setGraphicsSystem("raster");
    }
}

Qt::HANDLE MGraphicsSystemHelper::imageToEGLSharedImage(const QImage &image)
{
    ENSURE_RUNNING_MEEGO;
    return MRuntime::imageToEGLSharedImage(image);
}

QPixmap MGraphicsSystemHelper::pixmapFromEGLSharedImage(Qt::HANDLE handle, const QImage &softImage)
{
    // This function is supported when not running meego too. A raster-backed
    // pixmap will be created... but when you switch back to 'meego', it'll 
    // be replaced with a EGL shared image backing.
    return MRuntime::pixmapFromEGLSharedImage(handle, softImage);
}

QPixmap MGraphicsSystemHelper::pixmapWithGLTexture(int w, int h)
{
    ENSURE_RUNNING_MEEGO;
    return MRuntime::pixmapWithGLTexture(w, h);
}

bool MGraphicsSystemHelper::destroyEGLSharedImage(Qt::HANDLE handle)
{
    ENSURE_RUNNING_MEEGO;
    return MRuntime::destroyEGLSharedImage(handle);
}

void MGraphicsSystemHelper::updateEGLSharedImagePixmap(QPixmap *p)
{
    ENSURE_RUNNING_MEEGO;
    return MRuntime::updateEGLSharedImagePixmap(p);
}

void MGraphicsSystemHelper::setTranslucent(bool translucent)
{
    ENSURE_RUNNING_MEEGO;
    MRuntime::setTranslucent(translucent);
}
