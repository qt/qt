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
#ifndef QVNCCURSOR_H
#define QVNCCURSOR_H

#include <QList>
#include <QImage>
#include <QMouseEvent>

QT_BEGIN_NAMESPACE

class QVNCGraphicsSystemScreen;
class QVNCServer;

// Cursor graphics management
class QVNCCursorPrivate {
public:
    QVNCCursorPrivate(const uchar *data, const uchar *mask, int width, int height, int hotX, int hotY)
    { set(data, mask, width, height, hotX, hotY); }
    QImage * image() { return &cursorImage; }
    QPoint hotspot() { return hot; }
    QImage cursorImage;
    QPoint hot;
    void set(const uchar *data, const uchar *mask, int width, int height, int hotX, int hotY);
    void set(Qt::CursorShape);
private:
    static void createSystemCursor(int id);
};

class QVNCCursor {
public:
    QVNCCursor(QVNCServer *, QVNCGraphicsSystemScreen *);

    // input methods
    void setCursorMode(bool vnc);
    void setCursor(const uchar *data, const uchar *mask, int width, int height, int hotX, int hotY);
    void setCursor(Qt::CursorShape shape);
    void pointerEvent(QMouseEvent & event);

    // output methods
    QRect drawCursor(QPainter &);
    QRect dirtyRect();

    // VNC client communication
    void sendClientCursor();
    void clearClientCursor();
private:
    bool useVncCursor;      // VNC or local

    QRect currentRect;      // next place to draw the cursor
    QRect prevRect;         // last place the cursor was drawn

    QVNCServer * server;    // VNC server to get events from
    QVNCGraphicsSystemScreen * screen;  // Where to request an update
    QWidget * currentWidget; // widget currently under the cursor

    QVNCCursorPrivate * d_ptr;
};

QT_END_NAMESPACE

#endif // QVNCCURSOR_H
