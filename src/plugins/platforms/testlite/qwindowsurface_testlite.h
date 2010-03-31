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

#ifndef QWINDOWSURFACE_TESTLITE_H
#define QWINDOWSURFACE_TESTLITE_H

#include <QtGui/private/qwindowsurface_p.h>
#include <qevent.h>

QT_BEGIN_NAMESPACE

class MyWindow;
class QTestLiteIntegration;
class QTestLiteScreen;

class QTestLiteWindowSurface : public QWindowSurface
{
public:
    QTestLiteWindowSurface
        (QTestLiteIntegration *platformIntegration,
         QTestLiteScreen *screen, QWidget *window);
    ~QTestLiteWindowSurface();

    QPaintDevice *paintDevice();
    void flush(QWidget *widget, const QRegion &region, const QPoint &offset);
    void setGeometry(const QRect &rect);
    bool scroll(const QRegion &area, int dx, int dy);

    void beginPaint(const QRegion &region);
    void endPaint(const QRegion &region);

    void handleMouseEvent(QEvent::Type, void *); //forwarding X types is apparently impossible :(
    void handleKeyEvent(QEvent::Type, void *);
    void handleGeometryChange(int x, int y, int w, int h);
    void handleCloseEvent();
    void handleEnterEvent();
    void handleLeaveEvent();

    Qt::WindowFlags setWindowFlags(Qt::WindowFlags type);
    Qt::WindowFlags windowFlags() const;
    void setVisible(bool visible);
    WId winId() const;
    void raise();
    void lower();
    void setWindowTitle(const QString &title);

    void setCursor(QCursor * cursor);

private:
    QTestLiteIntegration *mPlatformIntegration;
    QTestLiteScreen *mScreen;
    Qt::WindowFlags window_flags;
    MyWindow *xw;
};

QT_END_NAMESPACE

#endif
