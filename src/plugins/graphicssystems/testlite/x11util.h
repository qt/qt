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

#ifndef MYX11UTIL_H
#define MYX11UTIL_H

#include <QObject>
#include <QImage>

#include <private/qt_x11_p.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>


class MyWindow;

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

public slots:
    void eventDispatcher();

public: //###
    Display * display;
    int screen;
    int width, height;
    int physicalWidth;
    int physicalHeight;

    QList<MyWindow*> windowList;
};

class QTestLiteWindowSurface; //### abstract callback interface, anyone?

class MyWindow : public QObject
{
    Q_OBJECT;
public:
    MyWindow(MyDisplay *xd, int x, int y, int w, int h);
    ~MyWindow();


    void mousePressEvent(XButtonEvent*);
    void mouseReleaseEvent(XButtonEvent*);
    void mouseMoveEvent(XButtonEvent*);

    void keyPressEvent(XKeyEvent*);
    void keyReleaseEvent(XKeyEvent*);

    void enterEvent(XCrossingEvent*);
    void leaveEvent(XCrossingEvent*);

    void closeEvent();
    void paintEvent();
    void resizeEvent(XConfigureEvent *configure_event);
//    void setSize(int w, int h);
    void setGeometry(int x, int y, int w, int h);

    GC createGC();

    Qt::WindowFlags setWindowFlags(Qt::WindowFlags flags);
    void setVisible(bool visible);

public: //###

    int xpos, ypos;
    int width, height;
    QImage img;
    Window window;
    MyDisplay *xd;
    GC gc;

    QTestLiteWindowSurface *windowSurface;
};





#endif // MYX11UTIL_H
