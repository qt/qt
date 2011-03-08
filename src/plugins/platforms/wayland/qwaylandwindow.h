/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the config.tests of the Qt Toolkit.
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

#ifndef QWAYLANDWINDOW_H
#define QWAYLANDWINDOW_H

#include <QtGui/QPlatformWindow>

#include <stdint.h>

class QWaylandDisplay;
class QWaylandBuffer;

class QWaylandWindow : public QPlatformWindow
{
public:
    QWaylandWindow(QWidget *window);
    ~QWaylandWindow();
    struct wl_surface *surface() { return mSurface; }

    void setVisible(bool visible);
    void configure(uint32_t time, uint32_t edges,
                   int32_t x, int32_t y, int32_t width, int32_t height);
    WId winId() const;
    void setParent(const QPlatformWindow *parent);
    QPlatformGLContext *glContext() const;
    void attach(QWaylandBuffer *buffer);
    QWaylandBuffer *getBuffer(void) { return mBuffer; }
    QWaylandWindow *getParentWindow(void) { return mParentWindow; }

private:
    struct wl_surface *mSurface;
    QWaylandDisplay *mDisplay;
    QPlatformGLContext *mGLContext;
    WId mWindowId;

    QWaylandBuffer *mBuffer;
    QWaylandWindow *mParentWindow;
};


#endif // QWAYLANDWINDOW_H
