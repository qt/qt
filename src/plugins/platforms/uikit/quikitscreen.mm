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

#include "quikitscreen.h"
#include "quikitwindow.h"

#include <QtGui/QApplication>

#include <QtDebug>

QT_BEGIN_NAMESPACE

QUIKitScreen::QUIKitScreen(int screenIndex)
    : QPlatformScreen(),
      m_index(screenIndex)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    CGRect bounds = [uiScreen() bounds];
    m_geometry = QRect(bounds.origin.x, bounds.origin.y, bounds.size.width, bounds.size.height);

    m_format = QImage::Format_ARGB32_Premultiplied;

    m_depth = 24;

    const qreal inch = 25.4;
    qreal dpi = 160.;
    int dragDistance = 12;
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad) {
        dpi = 132.;
        dragDistance = 10;
    }
    m_physicalSize = QSize(qRound(bounds.size.width * inch / dpi), qRound(bounds.size.height * inch / dpi));
    qApp->setStartDragDistance(dragDistance);

    QFont font; // system font is helvetica, so that is fine already
    font.setPixelSize([UIFont systemFontSize]);
    qApp->setFont(font);

    [pool release];
}

QUIKitScreen::~QUIKitScreen()
{
}

UIScreen *QUIKitScreen::uiScreen() const
{
    return [[UIScreen screens] objectAtIndex:m_index];
}

void QUIKitScreen::updateInterfaceOrientation()
{
    CGRect bounds = [uiScreen() bounds];
    switch ([[UIApplication sharedApplication] statusBarOrientation]) {
    case UIInterfaceOrientationPortrait:
    case UIInterfaceOrientationPortraitUpsideDown:
        m_geometry = QRect(bounds.origin.x, bounds.origin.y, bounds.size.width, bounds.size.height);;
        break;
    case UIInterfaceOrientationLandscapeLeft:
    case UIInterfaceOrientationLandscapeRight:
        m_geometry = QRect(bounds.origin.x, bounds.origin.y,
                           bounds.size.height, bounds.size.width);
        break;
    }
    foreach (QWidget *widget, qApp->topLevelWidgets()) {
        QUIKitWindow *platformWindow = static_cast<QUIKitWindow *>(widget->platformWindow());
        if (platformWindow && platformWindow->platformScreen() == this) {
            platformWindow->updateGeometryAndOrientation();
        }
    }
}

QT_END_NAMESPACE
