/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion <blackberry-qt@qnx.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QBBSCREEN_H
#define QBBSCREEN_H

#include <QtGui/QPlatformScreen>

#include "qbbrootwindow.h"

#include <screen/screen.h>

QT_BEGIN_NAMESPACE

class QBBWindow;

class QBBScreen : public QPlatformScreen
{
    Q_OBJECT
public:
    QBBScreen(screen_context_t context, screen_display_t display, int screenIndex);
    virtual ~QBBScreen();

    virtual QRect geometry() const { return mCurrentGeometry; }
    virtual QRect availableGeometry() const;
    virtual int depth() const;
    virtual QImage::Format format() const { return (depth() == 32) ? QImage::Format_RGB32 : QImage::Format_RGB16; }
    virtual QSize physicalSize() const { return mCurrentPhysicalSize; }

    int rotation() const { return mCurrentRotation; }

    int nativeFormat() const { return (depth() == 32) ? SCREEN_FORMAT_RGBA8888 : SCREEN_FORMAT_RGB565; }
    screen_display_t nativeDisplay() const { return mDisplay; }
    screen_context_t nativeContext() const { return mContext; }
    const char *windowGroupName() const { return mRootWindow->groupName().constData(); }

    QBBWindow *findWindow(screen_window_t windowHandle);

    /* Window hierarchy management */
    void addWindow(QBBWindow* child);
    void removeWindow(QBBWindow* child);
    void raiseWindow(QBBWindow* window);
    void lowerWindow(QBBWindow* window);
    void updateHierarchy();

    void onWindowPost(QBBWindow* window);
    void ensureDisplayCreated();

    QSharedPointer<QBBRootWindow> rootWindow() const { return mRootWindow; }

public Q_SLOTS:
    void setRotation(int rotation);
    void newWindowCreated(screen_window_t window);
    void windowClosed(screen_window_t window);
    void activateWindowGroup(const QByteArray &id);
    void deactivateWindowGroup(const QByteArray &id);

private Q_SLOTS:
    void keyboardHeightChanged(int height);

private:
    void resizeNativeWidgetWindow(QBBWindow *w, const QRect &previousScreenGeometry) const;
    void resizeTopLevelWindow(QBBWindow *w, const QRect &previousScreenGeometry) const;
    void resizeWindows(const QRect &previousScreenGeometry);
    void addOverlayWindow(screen_window_t window);
    void removeOverlayWindow(screen_window_t window);

    screen_context_t mContext;
    screen_display_t mDisplay;
    QSharedPointer<QBBRootWindow> mRootWindow;
    bool mPosted;
    bool mUsingOpenGL;
    bool mPrimaryDisplay;

    int mStartRotation;
    int mCurrentRotation;
    int mKeyboardHeight;
    QSize mStartPhysicalSize;
    QSize mCurrentPhysicalSize;
    QRect mStartGeometry;
    QRect mCurrentGeometry;

    QList<QBBWindow*> mChildren;
    QList<screen_window_t> mOverlays;
    int mScreenIndex;

    bool isPrimaryDisplay() { return mPrimaryDisplay; }
};

QT_END_NAMESPACE

#endif // QBBSCREEN_H
