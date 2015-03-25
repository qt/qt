/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion <blackberry-qt@qnx.com>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QBBWINDOW_H
#define QBBWINDOW_H

#include "qbbbuffer.h"

#include <QImage>
#include <QtGui/QPlatformWindow>

#include <screen/screen.h>
#include <EGL/egl.h>

QT_BEGIN_NAMESPACE

class QPlatformWindowFormat;
class QBBGLContext;
class QBBScreen;

class QBBWindow : public QPlatformWindow
{
friend class QBBScreen;
public:
    QBBWindow(QWidget *window, screen_context_t context, QBBScreen *screen);
    virtual ~QBBWindow();

    virtual void setGeometry(const QRect &rect);
    virtual void setVisible(bool visible);
    virtual void setOpacity(qreal level);

    virtual WId winId() const { return (WId)mWindow; }

    void setBufferSize(const QSize &size);
    QSize bufferSize() const { return mBufferSize; }
    bool hasBuffers() const { return !mBufferSize.isEmpty(); }

    QBBBuffer &renderBuffer();
    QBBBuffer &frontBuffer();

    void scroll(const QRegion &region, int dx, int dy, bool flush=false);
    void post(const QRegion &dirty);

    void setScreen(QBBScreen *platformScreen);

    virtual QPlatformGLContext *glContext() const;

    virtual void setParent(const QPlatformWindow *window);
    virtual void raise();
    virtual void lower();
    virtual void requestActivateWindow();

    void gainedFocus();

    QBBScreen* screen() const { return mScreen; }
    const QList<QBBWindow*>& children() const { return mChildren; }

    QBBWindow *findWindow(screen_window_t windowHandle);

private:

    enum Buffer {
        BACK_BUFFER,
        FRONT_BUFFER,
        MAX_BUFFER_COUNT
    };

    QBBBuffer &buffer(QBBWindow::Buffer bufferIndex);

    screen_context_t mContext;
    screen_window_t mWindow;
    QSize mBufferSize;
    QBBBuffer mBuffers[MAX_BUFFER_COUNT];
    int mCurrentBufferIndex;
    int mPreviousBufferIndex;
    QRegion mPreviousDirty;
    QRegion mScrolled;

    mutable QBBGLContext* mPlatformGlContext;
    QBBScreen* mScreen;
    QList<QBBWindow*> mChildren;
    QBBWindow *mParent;
    bool mVisible;

    void removeFromParent();
    void offset(const QPoint &offset);
    void updateVisibility(bool parentVisible);
    void updateZorder(int &topZorder);

    void fetchBuffers();

    void copyBack(const QRegion &region, int dx, int dy, bool flush=false);

    static int platformWindowFormatToNativeFormat(const QPlatformWindowFormat &format);
};

QT_END_NAMESPACE

#endif // QBBWINDOW_H
