/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QApplication>
#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QtTest/QtTest>

#include "qnativeevents.h"
#include "nativeeventlist.h"
#include "expectedeventlist.h"

#ifdef Q_OS_MAC

QT_USE_NAMESPACE

class tst_MacNativeEvents : public QObject
{
Q_OBJECT
private slots:
    void testMouseMoveLocation();
    void testPushButtonPressRelease();
    void testMouseLeftDoubleClick();
    void stressTestMouseLeftDoubleClick();
    void testMouseDragInside();
    void testMouseDragOutside();
    void testMouseDragToNonClientArea();
    void testDragWindow();
    void testMouseEnter();
    void testChildDialogInFrontOfModalParent();
    void testKeyPressOnToplevel();
};

void tst_MacNativeEvents::testMouseMoveLocation()
{
    QWidget w;
    w.setMouseTracking(true);
    w.show();
    QPoint p = w.geometry().center();

    NativeEventList native;
    native.append(new QNativeMouseMoveEvent(p, Qt::NoModifier));

    ExpectedEventList expected(&w);
    expected.append(new QMouseEvent(QEvent::MouseMove, w.mapFromGlobal(p), p, Qt::NoButton, Qt::NoButton, Qt::NoModifier));

    native.play();
    QVERIFY2(expected.waitForAllEvents(), "the test did not receive all expected events!");
}

void tst_MacNativeEvents::testPushButtonPressRelease()
{
    // Check that a native mouse press and release generates the
    // same qevents on a pushbutton:
    QPushButton w("click me");
    w.show();
    QPoint p = w.geometry().center();

    NativeEventList native;
    native.append(new QNativeMouseButtonEvent(p, Qt::LeftButton, 1, Qt::NoModifier));
    native.append(new QNativeMouseButtonEvent(p, Qt::LeftButton, 0, Qt::NoModifier));

    ExpectedEventList expected(&w);
    expected.append(new QMouseEvent(QEvent::MouseButtonPress, w.mapFromGlobal(p), p, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
    expected.append(new QMouseEvent(QEvent::MouseButtonRelease, w.mapFromGlobal(p), p, Qt::LeftButton, Qt::NoButton, Qt::NoModifier));

    native.play();
    QVERIFY2(expected.waitForAllEvents(), "the test did not receive all expected events!");
}

void tst_MacNativeEvents::testMouseLeftDoubleClick()
{
    // Check that a native double click makes
    // the test widget receive a press-release-click-release:
    QWidget w;
    w.show();
    QPoint p = w.geometry().center();

    NativeEventList native;
    native.append(new QNativeMouseButtonEvent(p, Qt::LeftButton, 1, Qt::NoModifier));
    native.append(new QNativeMouseButtonEvent(p, Qt::LeftButton, 0, Qt::NoModifier));
    native.append(new QNativeMouseButtonEvent(p, Qt::LeftButton, 2, Qt::NoModifier));
    native.append(new QNativeMouseButtonEvent(p, Qt::LeftButton, 0, Qt::NoModifier));

    ExpectedEventList expected(&w);
    expected.append(new QMouseEvent(QEvent::MouseButtonPress, w.mapFromGlobal(p), p, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
    expected.append(new QMouseEvent(QEvent::MouseButtonRelease, w.mapFromGlobal(p), p, Qt::LeftButton, Qt::NoButton, Qt::NoModifier));
    expected.append(new QMouseEvent(QEvent::MouseButtonDblClick, w.mapFromGlobal(p), p, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
    expected.append(new QMouseEvent(QEvent::MouseButtonRelease, w.mapFromGlobal(p), p, Qt::LeftButton, Qt::NoButton, Qt::NoModifier));

    native.play();
    QVERIFY2(expected.waitForAllEvents(), "the test did not receive all expected events!");
}

void tst_MacNativeEvents::stressTestMouseLeftDoubleClick()
{
    // Check that multiple, fast, double clicks makes
    // the test widget receive correct click events
    QWidget w;
    w.show();
    QPoint p = w.geometry().center();

    NativeEventList native;
    ExpectedEventList expected(&w);

    for (int i=0; i<10; ++i){
        native.append(new QNativeMouseButtonEvent(p, Qt::LeftButton, 1, Qt::NoModifier));
        native.append(new QNativeMouseButtonEvent(p, Qt::LeftButton, 0, Qt::NoModifier));
        native.append(new QNativeMouseButtonEvent(p, Qt::LeftButton, 2, Qt::NoModifier));
        native.append(new QNativeMouseButtonEvent(p, Qt::LeftButton, 0, Qt::NoModifier));

        expected.append(new QMouseEvent(QEvent::MouseButtonPress, w.mapFromGlobal(p), p, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
        expected.append(new QMouseEvent(QEvent::MouseButtonRelease, w.mapFromGlobal(p), p, Qt::LeftButton, Qt::NoButton, Qt::NoModifier));
        expected.append(new QMouseEvent(QEvent::MouseButtonDblClick, w.mapFromGlobal(p), p, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
        expected.append(new QMouseEvent(QEvent::MouseButtonRelease, w.mapFromGlobal(p), p, Qt::LeftButton, Qt::NoButton, Qt::NoModifier));
    }

    native.play();
    QVERIFY2(expected.waitForAllEvents(), "the test did not receive all expected events!");
}

void tst_MacNativeEvents::testMouseDragInside()
{
    // Check that a mouse drag inside a widget
    // will cause press-move-release events to be delivered
    QWidget w;
    w.show();
    QPoint p1 = w.geometry().center();
    QPoint p2 = p1 - QPoint(10, 0);
    QPoint p3 = p1 - QPoint(20, 0);
    QPoint p4 = p1 - QPoint(30, 0);

    NativeEventList native;
    native.append(new QNativeMouseButtonEvent(p1, Qt::LeftButton, 1, Qt::NoModifier));
    native.append(new QNativeMouseDragEvent(p2, Qt::LeftButton, Qt::NoModifier));
    native.append(new QNativeMouseDragEvent(p3, Qt::LeftButton, Qt::NoModifier));
    native.append(new QNativeMouseButtonEvent(p4, Qt::LeftButton, 0, Qt::NoModifier));

    ExpectedEventList expected(&w);
    expected.append(new QMouseEvent(QEvent::MouseButtonPress, w.mapFromGlobal(p1), p1, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
    expected.append(new QMouseEvent(QEvent::MouseMove, w.mapFromGlobal(p2), p2, Qt::NoButton, Qt::LeftButton, Qt::NoModifier));
    expected.append(new QMouseEvent(QEvent::MouseMove, w.mapFromGlobal(p3), p3, Qt::NoButton, Qt::LeftButton, Qt::NoModifier));
    expected.append(new QMouseEvent(QEvent::MouseButtonRelease, w.mapFromGlobal(p4), p4, Qt::LeftButton, Qt::NoButton, Qt::NoModifier));

    native.play();
    QVERIFY2(expected.waitForAllEvents(), "the test did not receive all expected events!");
}

void tst_MacNativeEvents::testMouseDragOutside()
{
    // Check that if we drag the mouse from inside the
    // widget, and release it outside, we still get mouse move
    // and release events when the mouse is outside the widget.
    QWidget w;
    w.show();
    QPoint inside1 = w.geometry().center();
    QPoint inside2 = inside1 - QPoint(10, 0);
    QPoint outside1 = w.geometry().topLeft() - QPoint(50, 0);
    QPoint outside2 = outside1 - QPoint(10, 0);

    NativeEventList native;
    native.append(new QNativeMouseButtonEvent(inside1, Qt::LeftButton, 1, Qt::NoModifier));
    native.append(new QNativeMouseDragEvent(inside2, Qt::LeftButton, Qt::NoModifier));
    native.append(new QNativeMouseDragEvent(outside1, Qt::LeftButton, Qt::NoModifier));
    native.append(new QNativeMouseButtonEvent(outside2, Qt::LeftButton, 0, Qt::NoModifier));

    ExpectedEventList expected(&w);
    expected.append(new QMouseEvent(QEvent::MouseButtonPress, w.mapFromGlobal(inside1), inside1, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
    expected.append(new QMouseEvent(QEvent::MouseMove, w.mapFromGlobal(inside2), inside2, Qt::NoButton, Qt::LeftButton, Qt::NoModifier));
    expected.append(new QMouseEvent(QEvent::MouseMove, w.mapFromGlobal(outside1), outside1, Qt::NoButton, Qt::LeftButton, Qt::NoModifier));
    expected.append(new QMouseEvent(QEvent::MouseButtonRelease, w.mapFromGlobal(outside2), outside2, Qt::LeftButton, Qt::NoButton, Qt::NoModifier));

    native.play();
    QVERIFY2(expected.waitForAllEvents(), "the test did not receive all expected events!");
}

void tst_MacNativeEvents::testMouseDragToNonClientArea()
{
    // Check that if we drag the mouse from inside the
    // widget, and release it on the title bar, we still get mouse move
    // and release events when the mouse is on the title bar
    QWidget w;
    w.show();
    QPoint inside1 = w.geometry().center();
    QPoint inside2 = inside1 - QPoint(10, 0);
    QPoint titlebar1 = w.geometry().topLeft() - QPoint(-100, 10);
    QPoint titlebar2 = titlebar1 - QPoint(10, 0);

    NativeEventList native;
    native.append(new QNativeMouseButtonEvent(inside1, Qt::LeftButton, 1, Qt::NoModifier));
    native.append(new QNativeMouseDragEvent(inside2, Qt::LeftButton, Qt::NoModifier));
    native.append(new QNativeMouseDragEvent(titlebar1, Qt::LeftButton, Qt::NoModifier));
    native.append(new QNativeMouseButtonEvent(titlebar2, Qt::LeftButton, 0, Qt::NoModifier));

    ExpectedEventList expected(&w);
    expected.append(new QMouseEvent(QEvent::MouseButtonPress, w.mapFromGlobal(inside1), inside1, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
    expected.append(new QMouseEvent(QEvent::MouseMove, w.mapFromGlobal(inside2), inside2, Qt::NoButton, Qt::LeftButton, Qt::NoModifier));
    expected.append(new QMouseEvent(QEvent::MouseMove, w.mapFromGlobal(titlebar1), titlebar1, Qt::NoButton, Qt::LeftButton, Qt::NoModifier));
    expected.append(new QMouseEvent(QEvent::MouseButtonRelease, w.mapFromGlobal(titlebar2), titlebar2, Qt::LeftButton, Qt::NoButton, Qt::NoModifier));

    native.play();
    QVERIFY2(expected.waitForAllEvents(), "the test did not receive all expected events!");
}

void tst_MacNativeEvents::testDragWindow()
{
    // Check that if we drag the mouse from inside the
    // widgets title bar, we get a move event on the window
    QWidget w;
    w.show();
    QPoint titlebar = w.geometry().topLeft() - QPoint(-100, 10);
    QPoint moveTo = titlebar + QPoint(100, 0);

    NativeEventList native;
    native.append(new QNativeMouseButtonEvent(titlebar, Qt::LeftButton, 1, Qt::NoModifier));
    native.append(new QNativeMouseDragEvent(moveTo, Qt::LeftButton, Qt::NoModifier));
    native.append(500, new QNativeMouseButtonEvent(moveTo, Qt::LeftButton, 0, Qt::NoModifier));

    ExpectedEventList expected(&w);
    expected.append(new QMouseEvent(QEvent::NonClientAreaMouseButtonPress, w.mapFromGlobal(titlebar), titlebar, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
    expected.append(new QMouseEvent(QEvent::NonClientAreaMouseButtonRelease, w.mapFromGlobal(titlebar), moveTo, Qt::LeftButton, Qt::NoButton, Qt::NoModifier));

    native.play();
    QVERIFY2(expected.waitForAllEvents(), "the test did not receive all expected events!");
}

void tst_MacNativeEvents::testMouseEnter()
{
    // When a mouse enters a widget, both a mouse enter events and a
    // mouse move event should be sendt. Lets test this:
    QWidget w;
    w.setMouseTracking(true);
    w.show();
    QPoint outside = w.geometry().topLeft() - QPoint(50, 0);
    QPoint inside = w.geometry().center();

    NativeEventList native;
    native.append(new QNativeMouseMoveEvent(outside, Qt::NoModifier));
    native.append(new QNativeMouseMoveEvent(inside, Qt::NoModifier));

    ExpectedEventList expected(&w);
    expected.append(new QEvent(QEvent::Enter));
    expected.append(new QMouseEvent(QEvent::MouseMove, w.mapFromGlobal(inside), inside, Qt::NoButton, Qt::NoButton, Qt::NoModifier));

    native.play();
    QVERIFY2(expected.waitForAllEvents(), "the test did not receive all expected events!");
}

void tst_MacNativeEvents::testChildDialogInFrontOfModalParent()
{
    // Test that a child dialog of a modal parent dialog is 
    // in front of the parent, and active:
    QDialog parent;
    parent.setWindowModality(Qt::ApplicationModal);
    QDialog child(&parent);
    QPushButton button("close", &child);
    connect(&button, SIGNAL(clicked()), &child, SLOT(close()));
    parent.show();
    child.show();
    QPoint inside = button.mapToGlobal(button.geometry().center());

    // Post a click on the button to close the child dialog:
    NativeEventList native;
    native.append(new QNativeMouseButtonEvent(inside, Qt::LeftButton, 1, Qt::NoModifier));
    native.append(new QNativeMouseButtonEvent(inside, Qt::LeftButton, 0, Qt::NoModifier));

    native.play();
    QTest::qWait(100);
    QVERIFY(!child.isVisible());
}

void tst_MacNativeEvents::testKeyPressOnToplevel()
{
    // Check that we receive keyevents for
    // toplevel widgets. For leagacy reasons, and according to Qt on
    // other platforms (carbon port + linux), we should get these events
    // even when the focus policy is set to Qt::NoFocus when there is no
    // other focus widget on screen:
    QWidget w;
    w.show();

    NativeEventList native;
    native.append(new QNativeKeyEvent(QNativeKeyEvent::Key_A, true, Qt::NoModifier));
    native.append(new QNativeKeyEvent(QNativeKeyEvent::Key_A, false, Qt::NoModifier));

    ExpectedEventList expected(&w);
    expected.append(new QKeyEvent(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier));
    expected.append(new QKeyEvent(QEvent::KeyRelease, Qt::Key_A, Qt::NoModifier));

    native.play();
    QVERIFY2(expected.waitForAllEvents(), "the test did not receive all expected events!");
}

#include "tst_macnativeevents.moc"

QTEST_MAIN(tst_MacNativeEvents)

#else

QTEST_NOOP_MAIN

#endif
