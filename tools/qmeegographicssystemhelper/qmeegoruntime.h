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

#include <QtGui/QPixmap>
#include <QtGui/QImage>

#include "QtMeeGoGraphicsSystemHelper/qmeegographicssystemhelper.h"

class QMeeGoRuntime
{
public:
    static void initialize();

    static Qt::HANDLE imageToEGLSharedImage(const QImage &image);
    static QPixmapData* pixmapDataFromEGLSharedImage(Qt::HANDLE handle, const QImage &softImage);
    static QPixmapData* pixmapDataWithGLTexture(int w, int h);
    static bool destroyEGLSharedImage(Qt::HANDLE handle);
    static void updateEGLSharedImagePixmap(QPixmap *p);
    static void setSurfaceFixedSize(int w, int h);
    static void setSurfaceScaling(int x, int y, int w, int h);
    static void setTranslucent(bool translucent);
    static QPixmapData* pixmapDataWithNewLiveTexture(int w, int h, QImage::Format format);
    static QPixmapData* pixmapDataFromLiveTextureHandle(Qt::HANDLE h);
    static QImage* lockLiveTexture(QPixmap *pixmap, void *fenceSync);
    static bool releaseLiveTexture(QPixmap *pixmap, QImage *image);
    static Qt::HANDLE getLiveTextureHandle(QPixmap *pixmap);
    static void* createFenceSync();
    static void destroyFenceSync(void *fs);
    static void invalidateLiveSurfaces();
    static void switchToRaster();
    static void switchToMeeGo();
    static void enableSwitchEvents();
    static void setSwitchPolicy(QMeeGoGraphicsSystemHelper::SwitchPolicy policy);

private:
    static bool initialized;
    static bool switchEventsEnabled;
};
