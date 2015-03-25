/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the config.tests of the Qt Toolkit.
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

#ifndef QWAYLANDWINDOW_H
#define QWAYLANDWINDOW_H

#include <QtGui/QPlatformWindow>
#include <QtCore/QWaitCondition>

#include "qwaylanddisplay.h"

class QWaylandDisplay;
class QWaylandBuffer;
struct wl_egl_window;

class QWaylandWindow : public QPlatformWindow
{
public:
    enum WindowType {
        Shm,
        Egl
    };

    QWaylandWindow(QWidget *window);
    ~QWaylandWindow();

    virtual WindowType windowType() const = 0;
    WId winId() const;
    void setVisible(bool visible);
    void setParent(const QPlatformWindow *parent);

    void configure(uint32_t time, uint32_t edges,
                   int32_t x, int32_t y, int32_t width, int32_t height);

    void attach(QWaylandBuffer *buffer);
    void damage(const QRect &rect);

    void waitForFrameSync();

    struct wl_surface *wl_surface() const { return mSurface; }

protected:
    struct wl_surface *mSurface;
    virtual void newSurfaceCreated();
    QWaylandDisplay *mDisplay;
    QWaylandBuffer *mBuffer;
    WId mWindowId;
    bool mWaitingForFrameSync;
    QWaitCondition mFrameSyncWait;

private:
    static void frameCallback(struct wl_surface *surface, void *data, uint32_t time);


};


#endif // QWAYLANDWINDOW_H
