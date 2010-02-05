/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

/****************************************************************************
 NB: This is not a header file, dispite the file name suffix. This file is
 included directly into the source code of qcocoawindow_mac.mm and
 qcocoapanel_mac.mm to avoid manually doing copy and paste of the exact
 same code needed at both places. This solution makes it more difficult
 to e.g fix a bug in qcocoawindow_mac.mm, but forget to do the same in
 qcocoapanel_mac.mm.
 The reason we need to do copy and paste in the first place, rather than
 resolve to method overriding, is that QCocoaPanel needs to inherit from
 NSPanel, while QCocoaWindow needs to inherit NSWindow rather than NSPanel).
****************************************************************************/

// WARNING: Don't include any header files from within this file. Put them
// directly into qcocoawindow_mac_p.h and qcocoapanel_mac_p.h

QT_BEGIN_NAMESPACE
extern Qt::MouseButton cocoaButton2QtButton(NSInteger buttonNum); // qcocoaview.mm
extern QPointer<QWidget> qt_button_down; //qapplication_mac.cpp
QT_END_NAMESPACE

- (BOOL)canBecomeKeyWindow
{
    QWidget *widget = [self QT_MANGLE_NAMESPACE(qt_qwidget)];

    bool isToolTip = (widget->windowType() == Qt::ToolTip);
    bool isPopup = (widget->windowType() == Qt::Popup);
    return !(isPopup || isToolTip);
}

- (void)toggleToolbarShown:(id)sender
{
    macSendToolbarChangeEvent([self QT_MANGLE_NAMESPACE(qt_qwidget)]);
    [super toggleToolbarShown:sender];
}

/*
    The methods keyDown, keyUp, and flagsChanged... These really shouldn't ever
    get hit. We automatically say we can be first responder if we are a window.
    So, the handling should get handled by the view. This is here more as a
    last resort (i.e., this is code that can potentially be removed).
 */
- (void)keyDown:(NSEvent *)theEvent
{
    bool keyOK = qt_dispatchKeyEvent(theEvent, [self QT_MANGLE_NAMESPACE(qt_qwidget)]);
    if (!keyOK)
        [super keyDown:theEvent];
}

- (void)keyUp:(NSEvent *)theEvent
{
    bool keyOK = qt_dispatchKeyEvent(theEvent, [self QT_MANGLE_NAMESPACE(qt_qwidget)]);
    if (!keyOK)
        [super keyUp:theEvent];
}

- (void)flagsChanged:(NSEvent *)theEvent
{
    qt_dispatchModifiersChanged(theEvent, [self QT_MANGLE_NAMESPACE(qt_qwidget)]);
    [super flagsChanged:theEvent];
}


- (void)tabletProximity:(NSEvent *)tabletEvent
{
    qt_dispatchTabletProximityEvent(tabletEvent);
}

- (void)sendEvent:(NSEvent *)event
{
    QWidget *widget = [[QT_MANGLE_NAMESPACE(QCocoaWindowDelegate) sharedDelegate] qt_qwidgetForWindow:self];

    // Cocoa can hold onto the window after we've disavowed its knowledge. So,
    // if we get sent an event afterwards just have it go through the super's
    // version and don't do any stuff with Qt.
    if (!widget) {
        [super sendEvent:event];
        return;
    }

    [self retain];
    QT_MANGLE_NAMESPACE(QCocoaView) *view = static_cast<QT_MANGLE_NAMESPACE(QCocoaView) *>(qt_mac_nativeview_for(widget));
    Qt::MouseButton mouseButton = cocoaButton2QtButton([event buttonNumber]);

    bool handled = false;
    // sometimes need to redirect mouse events to the popup.
    QWidget *popup = qAppInstance()->activePopupWidget();
    if (popup) {
        switch([event type])
        {
        case NSLeftMouseDown:
            if (!qt_button_down)
                qt_button_down = widget;
            handled = qt_mac_handleMouseEvent(view, event, QEvent::MouseButtonPress, mouseButton);
            // Don't call super here. This prevents us from getting the mouseUp event,
            // which we need to send even if the mouseDown event was not accepted. 
            // (this is standard Qt behavior.)
            break;
        case NSRightMouseDown:
        case NSOtherMouseDown:
            if (!qt_button_down)
                qt_button_down = widget;
            handled = qt_mac_handleMouseEvent(view, event, QEvent::MouseButtonPress, mouseButton);
            break;
        case NSLeftMouseUp:
        case NSRightMouseUp:
        case NSOtherMouseUp:
            handled = qt_mac_handleMouseEvent(view, event, QEvent::MouseButtonRelease, mouseButton);
            qt_button_down = 0;
            break;
        case NSMouseMoved:
            handled = qt_mac_handleMouseEvent(view, event, QEvent::MouseMove, Qt::NoButton);
            break;
        case NSLeftMouseDragged:
        case NSRightMouseDragged:
        case NSOtherMouseDragged:
            [QT_MANGLE_NAMESPACE(QCocoaView) currentMouseEvent]->view = view;
            [QT_MANGLE_NAMESPACE(QCocoaView) currentMouseEvent]->theEvent = event;
            handled = qt_mac_handleMouseEvent(view, event, QEvent::MouseMove, mouseButton);
            break;
        default:
            [super sendEvent:event];
            break;
        }
    } else {
        [super sendEvent:event];
    }

    if (!handled)
        qt_mac_dispatchNCMouseMessage(self, event, [self QT_MANGLE_NAMESPACE(qt_qwidget)], leftButtonIsRightButton);

    [self release];
}

- (BOOL)makeFirstResponder:(NSResponder *)responder
{
    // For some reason Cocoa wants to flip the first responder
    // when Qt doesn't want to, sorry, but "No" :-)
    if (responder == nil && qApp->focusWidget())
        return NO;
    return [super makeFirstResponder:responder];
}

+ (Class)frameViewClassForStyleMask:(NSUInteger)styleMask
{
    if (styleMask & QtMacCustomizeWindow)
        return [QT_MANGLE_NAMESPACE(QCocoaWindowCustomThemeFrame) class];
    return [super frameViewClassForStyleMask:styleMask];
}

- (void)displayIfNeeded
{

    QWidget *qwidget = [[QT_MANGLE_NAMESPACE(QCocoaWindowDelegate) sharedDelegate] qt_qwidgetForWindow:self];
    if (qwidget == 0) {
        [super displayIfNeeded];
        return;
    }

    if (QApplicationPrivate::graphicsSystem() != 0) {
        if (QWidgetBackingStore *bs = qt_widget_private(qwidget)->maybeBackingStore())
            bs->sync(qwidget, qwidget->rect());
    }
    [super displayIfNeeded];
}


