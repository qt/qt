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

#include "qcocoaintegration.h"

#include "qcocoawindow.h"
#include "qcocoawindowsurface.h"
#include "qcocoaeventloopintegration.h"

#include "qcoretextfontdatabase.h"

#include <QtGui/QApplication>

#include <private/qpixmap_raster_p.h>

QT_BEGIN_NAMESPACE

QCocoaScreen::QCocoaScreen(int screenIndex)
    :QPlatformScreen()
{
    m_screen = [[NSScreen screens] objectAtIndex:screenIndex];
    NSRect rect = [m_screen frame];
    m_geometry = QRect(rect.origin.x,rect.origin.y,rect.size.width,rect.size.height);

    m_format = QImage::Format_ARGB32;

    m_depth = NSBitsPerPixelFromDepth([m_screen depth]);

    const int dpi = 72;
    const qreal inch = 25.4;
    m_physicalSize = QSize(qRound(m_geometry.width() * inch / dpi), qRound(m_geometry.height() *inch / dpi));
}

QCocoaScreen::~QCocoaScreen()
{
}

QCocoaIntegration::QCocoaIntegration()
    : mFontDb(new QCoreTextFontDatabase())
{
    mPool = new QCocoaAutoReleasePool;

    //Make sure we have a nsapplication :)
    [NSApplication sharedApplication];
//    [[OurApplication alloc] init];

    NSArray *screens = [NSScreen screens];
    for (uint i = 0; i < [screens count]; i++) {
        QCocoaScreen *screen = new QCocoaScreen(i);
        mScreens.append(screen);
    }
}

QCocoaIntegration::~QCocoaIntegration()
{
    delete mPool;
}

bool QCocoaIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
    switch (cap) {
    case ThreadedPixmaps: return true;
    default: return QPlatformIntegration::hasCapability(cap);
    }
}



QPixmapData *QCocoaIntegration::createPixmapData(QPixmapData::PixelType type) const
{
    return new QRasterPixmapData(type);
}

QPlatformWindow *QCocoaIntegration::createPlatformWindow(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
    return new QCocoaWindow(widget);
}

QWindowSurface *QCocoaIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
    return new QCocoaWindowSurface(widget,winId);
}

QPlatformFontDatabase *QCocoaIntegration::fontDatabase() const
{
    return mFontDb;
}

QPlatformEventLoopIntegration *QCocoaIntegration::createEventLoopIntegration() const
{
    return new QCocoaEventLoopIntegration();
}
QT_END_NAMESPACE
