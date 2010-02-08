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
#include <qtimer.h>
#include <QDateTime>

#include <private/qt_x11_p.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>


class MyWindow;
class MyX11Cursors;

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

    QList<MyWindow*> windowList;

    MyX11Cursors * cursors;
};

class QTestLiteWindowSurface; //### abstract callback interface, anyone?

struct MyShmImageInfo;

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
    void setCursor(QCursor * cursor);
    Cursor createCursorShape(int cshape);
    Cursor createCursorBitmap(QCursor * cursor);

    void setWindowTitle(const QString &title);

    QImage *image() { return &shm_img; }

public: //###

    int xpos, ypos;
    int width, height;
    Window window;
    MyDisplay *xd;
    GC gc;

    QTestLiteWindowSurface *windowSurface;

    int currentCursor;
    bool painted;
    void resizeBuffer(QSize);
    QSize bufferSize() const;

private:
    void resizeShmImage(int width, int height);

    QImage shm_img;
    MyShmImageInfo *image_info;
};

class MyX11CursorNode
{
public:
    MyX11CursorNode(int id, Cursor c) { idValue = id; cursorValue = c; refCount = 1; }
    QDateTime expiration() { return t; }
    void setExpiration(QDateTime val) { t = val; }
    MyX11CursorNode * ante() { return before; }
    void setAnte(MyX11CursorNode *node) { before = node; }
    MyX11CursorNode * post() { return after; }
    void setPost(MyX11CursorNode *node) { after = node; }
    Cursor cursor() { return cursorValue; }
    int id() { return idValue; }
    unsigned int refCount;

private:
    MyX11CursorNode *before;
    MyX11CursorNode *after;
    QDateTime t;
    Cursor cursorValue;
    int idValue;

    Display * display;
};

class MyX11Cursors : public QObject
{
    Q_OBJECT
public:
    MyX11Cursors(Display * d);
    ~MyX11Cursors() { timer.stop(); }
    void incrementUseCount(int id);
    void decrementUseCount(int id);
    void createNode(int id, Cursor c);
    bool exists(int id) { return lookupMap.contains(id); }
    Cursor cursor(int id);
public slots:
    void timeout();

private:
    void removeNode(MyX11CursorNode *node);
    void insertNode(MyX11CursorNode *node);

    // linked list of cursors currently not assigned to any window
    MyX11CursorNode *firstExpired;
    MyX11CursorNode *lastExpired;

    QHash<int, MyX11CursorNode *> lookupMap;
    QTimer timer;

    Display *display;

    int removalDelay;
};

#endif // MYX11UTIL_H
