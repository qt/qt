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

#include "qvncintegration.h"
#include "../fb_base/fb_base.h"
#include <private/qapplication_p.h>
#include <QtGui/private/qpixmap_raster_p.h>
#include <QtCore/qdebug.h>

#include <qvncserver.h>
#include <QtGui/QPainter>

#include <QtCore/QTimer>


QVNCScreen::QVNCScreen(QRect screenSize, int screenId)
        : QFbScreen::QFbScreen()
{
    setGeometry(screenSize);
    setDepth(32);
    setFormat(QImage::Format_RGB32);
    setPhysicalSize((geometry().size()*254)/720);


    d_ptr = new QVNCScreenPrivate(this, screenId);

    cursor = new QVNCCursor(d_ptr->vncServer, this);
    d_ptr->vncServer->setCursor(static_cast<QVNCCursor *>(cursor));
}

QVNCDirtyMap *QVNCScreen::dirtyMap()
{
    return d_ptr->dirty;
}

QRegion QVNCScreen::doRedraw()
{
    QRegion touched;
    touched = QFbScreen::doRedraw();

    QVector<QRect> rects = touched.rects();
    for (int i = 0; i < rects.size(); i++)
        d_ptr->setDirty(rects[i]);
    return touched;
}

static inline int defaultWidth() { return 800; }
static inline int defaultHeight() { return 600; }
static inline int defaultDisplay() { return 0; }

static void usage()
{
    qWarning() << "VNC Platform Integration options:";
    qWarning() << "    size=<Width>x<Height> - set the display width and height";
    qWarning() << "         defaults to" << defaultWidth() << "x" << defaultHeight();
    qWarning() << "    display=<ID> - set the VNC display port to ID + 5900";
    qWarning() << "         defaults to" << defaultDisplay();
}

QVNCIntegration::QVNCIntegration(const QStringList& paramList)
{
    int sizeX = defaultWidth();
    int sizeY = defaultHeight();
    int display = defaultDisplay();
    bool showUsage = false;

    foreach(QString confString, paramList) {
        if (confString.startsWith(QLatin1String("size="))) {
            QString val = confString.section(QLatin1Char('='), 1, 1);
            sizeX = val.section(QLatin1Char('x'), 0, 0).toInt();
            sizeY = val.section(QLatin1Char('x'), 1, 1).toInt();
        }
        else if (confString.startsWith(QLatin1String("display="))) {
            display = confString.section(QLatin1Char('='), 1, 1).toInt();
        }
        else {
            qWarning() << "Unknown VNC option:" << confString;
            showUsage = true;
        }
    }

    if (showUsage)
        usage();

    mPrimaryScreen = new QVNCScreen(QRect(0, 0, sizeX, sizeY), display);

    mScreens.append(mPrimaryScreen);
}

QPixmapData *QVNCIntegration::createPixmapData(QPixmapData::PixelType type) const
{
    return new QRasterPixmapData(type);
}

// QWindowSurface *QVNCIntegration::createWindowSurface(QWidget *widget) const
// {
//     if (widget->windowType() == Qt::Desktop)
//         return 0;   // Don't create an explicit window surface for the destkop.
//     QFbWindowSurface * surface;
//     surface = new QFbWindowSurface(mPrimaryScreen, widget);
//     mPrimaryScreen->addWindowSurface(surface);
//     return surface;
// }

QWindowSurface *QVNCIntegration::createWindowSurface(QWidget *widget, WId) const
{
    QFbWindowSurface * surface;
    surface = new QFbWindowSurface(mPrimaryScreen, widget);
    return surface;
}


QPlatformWindow *QVNCIntegration::createPlatformWindow(QWidget *widget, WId /*winId*/) const
{
    QFbWindow *w = new QFbWindow(mPrimaryScreen, widget);
    mPrimaryScreen->addWindow(w);
    return w;
}

