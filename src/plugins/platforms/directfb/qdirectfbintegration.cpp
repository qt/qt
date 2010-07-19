/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qdirectfbintegration.h"
#include "qdirectfbwindowsurface.h"
#include "qdirectfbblitter.h"
#include "qdirectfbconvenience.h"
#include "qdirectfbcursor.h"
#include "qdirectfbwindow.h"

#include <private/qwindowsurface_raster_p.h>
#include <private/qpixmap_raster_p.h>

#include <private/qpixmap_blitter_p.h>
#include <private/qpixmapdata_p.h>
#include <QCoreApplication>

QT_BEGIN_NAMESPACE

QDirectFbScreen::QDirectFbScreen(int display)
    :QPlatformScreen()
{
    m_layer = QDirectFbConvenience::dfbDisplayLayer(display);
    m_layer->SetCooperativeLevel(m_layer,DLSCL_SHARED);

    DFBDisplayLayerConfig config;
    m_layer->GetConfiguration(m_layer, &config);

    m_format = QDirectFbConvenience::imageFormatFromSurfaceFormat(config.pixelformat, config.surface_caps);
    m_geometry = QRect(0,0,config.width,config.height);
    const int dpi = 72;
    const qreal inch = 25.4;
    m_depth = 32;
    m_physicalSize = QSize(qRound(config.width * inch / dpi), qRound(config.height *inch / dpi));

    cursor = new QDirectFBCursor(this);
}

QDirectFbScreen::~QDirectFbScreen()
{
}

QDirectFbIntegration::QDirectFbIntegration()
{
    const QStringList args = QCoreApplication::arguments();
    int argc = args.size();
    char **argv = new char*[argc];

    for (int i = 0; i < argc; ++i)
        argv[i] = qstrdup(args.at(i).toLocal8Bit().constData());

    DFBResult result = DirectFBInit(&argc, &argv);
    if (result != DFB_OK) {
        DirectFBError("QDirectFBScreen: error initializing DirectFB",
                      result);
    }
    delete[] argv;

    QDirectFbScreen *primaryScreen = new QDirectFbScreen(0);
    mScreens.append(primaryScreen);
}

QPixmapData *QDirectFbIntegration::createPixmapData(QPixmapData::PixelType type) const
{
    if (type == QPixmapData::BitmapType)
        return new QRasterPixmapData(type);
    else
        return new QBlittablePixmapData(type);
}

QPlatformWindow *QDirectFbIntegration::createPlatformWindow(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
    return new QDirectFbWindow(widget);
}

QWindowSurface *QDirectFbIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
    return new QDirectFbWindowSurface(widget,winId);
}

QBlittable *QDirectFbIntegration::createBlittable(const QSize &size) const
{
    return new QDirectFbBlitter(size);
}

QT_END_NAMESPACE
