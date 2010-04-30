/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenVG module of the Qt Toolkit.
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

#ifndef QTESTLITEWINDOW_H
#define QTESTLITEWINDOW_H

#include <QPlatformWindow>
#include <qevent.h>

#include <QObject>
#include <QImage>
#include <qtimer.h>
#include <QDateTime>

#include <private/qt_x11_p.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>



class QTestLiteIntegration;
class QTestLiteScreen;
class QTestLiteWindowSurface;
class MyX11Cursors;
class QTestLiteWindow;

class MyDisplay : public QObject
{
    Q_OBJECT;
public:
    MyDisplay();
    ~MyDisplay();

    Window rootWindow() { return RootWindow(display, screen); }
    unsigned long blackPixel() { return BlackPixel(display, screen); }
    unsigned long whitePixel() { return WhitePixel(display, screen); }

    bool handleEvent(XEvent *xe);
    QImage grabWindow(Window window, int x, int y, int w, int h);

public slots:
    void eventDispatcher();

public: //###
    Display * display;
    int screen;
    int width, height;
    int physicalWidth;
    int physicalHeight;

    QList<QTestLiteWindow*> windowList;

    MyX11Cursors * cursors;
};

struct MyShmImageInfo;

class QTestLiteWindow : public QPlatformWindow
{
public:
    QTestLiteWindow(const QTestLiteIntegration *platformIntegration,
         QTestLiteScreen *screen, QWidget *window);
    ~QTestLiteWindow();


    void mousePressEvent(XButtonEvent*);
    void handleMouseEvent(QEvent::Type, XButtonEvent *ev);

    void handleKeyEvent(QEvent::Type, void *);
    void handleCloseEvent();
    void handleEnterEvent();
    void handleLeaveEvent();

    void resizeEvent(XConfigureEvent *configure_event);
    void paintEvent();


    void setGeometry(const QRect &rect);

    Qt::WindowFlags setWindowFlags(Qt::WindowFlags type);
    Qt::WindowFlags windowFlags() const;
    void setVisible(bool visible);
    WId winId() const;
    void raise();
    void lower();
    void setWindowTitle(const QString &title);

    void setCursor(QCursor * cursor);

private:
    int xpos, ypos;
    int width, height;
    Window x_window;
    GC gc;

    GC createGC();
    Cursor createCursorShape(int cshape);
    Cursor createCursorBitmap(QCursor * cursor);

    int currentCursor;

    MyDisplay *xd;

    QTestLiteScreen *mScreen;
    Qt::WindowFlags window_flags;

    friend class QTestLiteWindowSurface; // x_window, gc and windowSurface
};





#endif
