/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QGRAPHICSSYSTEM_TESTLITE_H
#define QGRAPHICSSYSTEM_TESTLITE_H

#include <QtGui/QPlatformIntegration>
#include <QtGui/QPlatformScreen>

QT_BEGIN_NAMESPACE

class MyDisplay;

class QTestLiteScreen : public QPlatformScreen
{
public:
    QTestLiteScreen()
        : mDepth(16), mFormat(QImage::Format_RGB16) {}
    ~QTestLiteScreen() {}

    QRect geometry() const { return mGeometry; }
    int depth() const { return mDepth; }
    QImage::Format format() const { return mFormat; }
    QSize physicalSize() const { return mPhysicalSize; }

public:
    QRect mGeometry;
    int mDepth;
    QImage::Format mFormat;
    QSize mPhysicalSize;
};

class QTestLiteIntegration : public QPlatformIntegration
{
public:
    QTestLiteIntegration();

    QPixmapData *createPixmapData(QPixmapData::PixelType type) const;
    QPlatformWindow *createPlatformWindow(QWidget *widget, WId winId) const;
    QWindowSurface *createWindowSurface(QWidget *widget, WId winId) const;

    QPixmap grabWindow(WId window, int x, int y, int width, int height) const;

    QList<QPlatformScreen *> screens() const { return mScreens; }

#ifndef QT_NO_OPENGL
    bool hasOpenGL() const;
    QPlatformGLContext * createGLContext();
#endif

    MyDisplay *xd;

private:
    QTestLiteScreen *mPrimaryScreen;
    QList<QPlatformScreen *> mScreens;
};

QT_END_NAMESPACE

#endif
