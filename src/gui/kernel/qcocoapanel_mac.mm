/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#import <private/qcocoapanel_mac_p.h>
#ifdef QT_MAC_USE_COCOA
#import <private/qt_cocoa_helpers_mac_p.h>
#import <private/qcocoawindow_mac_p.h>
#import <private/qcocoawindowcustomthemeframe_mac_p.h>

#include <QtGui/QWidget>

QT_USE_NAMESPACE

@implementation QT_MANGLE_NAMESPACE(QCocoaPanel)

- (BOOL)canBecomeKeyWindow
{
    QWidget *widget = [self QT_MANGLE_NAMESPACE(qt_qwidget)];

    bool isToolTip = (widget->windowType() == Qt::ToolTip);
    bool isPopup = (widget->windowType() == Qt::Popup);
    return !(isPopup || isToolTip);
}

- (void)sendEvent:(NSEvent *)event
{
    [self retain];
    [super sendEvent:event];
    qt_mac_dispatchNCMouseMessage(self, event, [self QT_MANGLE_NAMESPACE(qt_qwidget)], leftButtonIsRightButton);
    [self release];
}

+ (Class)frameViewClassForStyleMask:(NSUInteger)styleMask
{
    if (styleMask & QtMacCustomizeWindow)
        return [QCocoaWindowCustomThemeFrame class];
    return [super frameViewClassForStyleMask:styleMask];
}

@end
#endif
