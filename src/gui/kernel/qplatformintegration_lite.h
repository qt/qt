/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QPLATFORMINTEGRATION_H
#define QPLATFORMINTEGRATION_H

#include <QtGui/private/qgraphicssystem_p.h>
#include <QtGui/qplatformscreen_lite.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_OPENGL
class QPlatformGLContext;
class QPlatformGLWidgetSurface;
#endif

class Q_GUI_EXPORT QPlatformIntegration
{
public:
    virtual ~QPlatformIntegration() { };

// GraphicsSystem functions
    virtual QPixmapData *createPixmapData(QPixmapData::PixelType type) const = 0;
    virtual QPlatformWindow *createPlatformWindow(QWidget *widget, WId winId = 0) const = 0;
    virtual QWindowSurface *createWindowSurface(QWidget *widget, WId winId) const = 0;
    virtual QBlittable *createBlittable(const QSize &size) const;

// Window System functions
    virtual QList<QPlatformScreen *> screens() const = 0;
    virtual QPixmap grabWindow(WId window, int x, int y, int width, int height) const;

// OpenGL Integration functions
#ifndef QT_NO_OPENGL
    virtual bool hasOpenGL() const;
    virtual QPlatformGLContext       * createGLContext();
    virtual QPlatformGLWidgetSurface * createGLWidgetSurface();
#endif
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPLATFORMINTEGRATION_H
