/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion
**
** Contact: Research In Motion <blackberry-qt@qnx.com>
** Contact: Klarälvdalens Datakonsult AB <info@kdab.com>
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
**
**
**
**
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

// all surfaces double buffered
#define MAX_BUFFER_COUNT    2

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
