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
    CGFloat scale = [uiScreen() scale];
    updateInterfaceOrientation();

    m_format = QImage::Format_ARGB32_Premultiplied;

    m_depth = 24;

    const qreal inch = 25.4;
    qreal unscaledDpi = 160.;
    int dragDistance = 12 * scale;
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad) {
        unscaledDpi = 132.;
        dragDistance = 10 * scale;
    }
    m_physicalSize = QSize(qRound(bounds.size.width * inch / unscaledDpi), qRound(bounds.size.height * inch / unscaledDpi));
    qApp->setStartDragDistance(dragDistance);

    QFont font; // system font is helvetica, so that is fine already
    font.setPixelSize([UIFont systemFontSize] * scale);
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
    CGFloat scale = [uiScreen() scale];
    switch ([[UIApplication sharedApplication] statusBarOrientation]) {
    case UIInterfaceOrientationPortrait:
    case UIInterfaceOrientationPortraitUpsideDown:
        m_geometry = QRect(bounds.origin.x * scale, bounds.origin.y * scale,
                           bounds.size.width * scale, bounds.size.height * scale);;
        break;
    case UIInterfaceOrientationLandscapeLeft:
    case UIInterfaceOrientationLandscapeRight:
        m_geometry = QRect(bounds.origin.x * scale, bounds.origin.y * scale,
                           bounds.size.height * scale, bounds.size.width * scale);
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
