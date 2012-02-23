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

#ifndef QBBSCREEN_H
#define QBBSCREEN_H

#include <QtGui/QPlatformScreen>
#include <QByteArray>

#include "qbbrootwindow.h"

#include <screen/screen.h>

QT_BEGIN_NAMESPACE

class QBBWindow;

class QBBScreen : public QPlatformScreen
{
public:
    static QList<QPlatformScreen *> screens() { return sScreens; }
    static void createDisplays(screen_context_t context);
    static void destroyDisplays();
    static QBBScreen* primaryDisplay() { return static_cast<QBBScreen*>(sScreens.at(0)); }
    static int defaultDepth();

    virtual QRect geometry() const { return mCurrentGeometry; }
    virtual QRect availableGeometry() const;
    virtual int depth() const { return defaultDepth(); }
    virtual QImage::Format format() const { return (depth() == 32) ? QImage::Format_RGB32 : QImage::Format_RGB16; }
    virtual QSize physicalSize() const { return mCurrentPhysicalSize; }

    int rotation() const { return mCurrentRotation; }
    void setRotation(int rotation);

    int nativeFormat() const { return (depth() == 32) ? SCREEN_FORMAT_RGBA8888 : SCREEN_FORMAT_RGB565; }
    screen_display_t nativeDisplay() const { return mDisplay; }
    screen_context_t nativeContext() const { return mContext; }
    const char *windowGroupName() const { return mRootWindow->groupName().constData(); }

    /* Window hierarchy management */
    static void addWindow(QBBWindow* child);
    static void removeWindow(QBBWindow* child);
    static void raiseWindow(QBBWindow* window);
    static void lowerWindow(QBBWindow* window);
    static void updateHierarchy();

    void onWindowPost(QBBWindow* window);
    void ensureDisplayCreated();

    QSharedPointer<QBBRootWindow> rootWindow() const { return mRootWindow; }

private:
    screen_context_t mContext;
    screen_display_t mDisplay;
    QSharedPointer<QBBRootWindow> mRootWindow;
    bool mPosted;
    bool mUsingOpenGL;
    bool mPrimaryDisplay;

    int mStartRotation;
    int mCurrentRotation;
    QSize mStartPhysicalSize;
    QSize mCurrentPhysicalSize;
    QRect mStartGeometry;
    QRect mCurrentGeometry;

    static QList<QPlatformScreen *> sScreens;
    static QList<QBBWindow*> sChildren;

    QBBScreen(screen_context_t context, screen_display_t display, bool primary);
    virtual ~QBBScreen();

    bool isPrimaryDisplay() { return mPrimaryDisplay; }
};

QT_END_NAMESPACE

#endif // QBBSCREEN_H
