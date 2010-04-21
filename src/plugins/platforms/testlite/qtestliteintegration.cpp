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



#include "qtestliteintegration.h"
#include "qtestlitewindowsurface.h"
#include <QtGui/private/qpixmap_raster_p.h>
#include <QtCore/qdebug.h>

#include <QGraphicsSystemCursor>

#include "qtestlitewindow.h"

#ifndef QT_NO_OPENGL
#include <GL/glx.h>
#include "qglxglcontext.h"
#endif

QT_BEGIN_NAMESPACE

class MyCursor : QGraphicsSystemCursor
{
public:
    MyCursor(QPlatformScreen *screen) : QGraphicsSystemCursor(screen) {}

    void changeCursor(QCursor * cursor, QWidget * widget) {
        QTestLiteWindow *w = 0;
        if (widget) {
            QWidget *window = widget->window();
            w = static_cast<QTestLiteWindow*>(window->platformWindow());
        } else {
            // No X11 cursor control when there is no widget under the cursor
            return;
        }

        //qDebug() << "changeCursor" << widget << ws;
        if (!w)
            return;

        w->setCursor(cursor);
    }
};


QTestLiteIntegration::QTestLiteIntegration()
{

    xd = new MyDisplay;

    mPrimaryScreen = new QTestLiteScreen();

    mPrimaryScreen->mGeometry = QRect
        (0, 0, xd->width, xd->height);
    mPrimaryScreen->mDepth = 32;
    mPrimaryScreen->mFormat = QImage::Format_RGB32;
    mPrimaryScreen->mPhysicalSize =
        QSize(xd->physicalWidth, xd->physicalHeight);

    mScreens.append(mPrimaryScreen);


    (void)new MyCursor(mPrimaryScreen);

}

QPixmapData *QTestLiteIntegration::createPixmapData(QPixmapData::PixelType type) const
{
    return new QRasterPixmapData(type);
}

QWindowSurface *QTestLiteIntegration::createWindowSurface(QWidget *widget, WId) const
{
    return new QTestLiteWindowSurface(mPrimaryScreen, widget);
}


QPlatformWindow *QTestLiteIntegration::createPlatformWindow(QWidget *widget, WId /*winId*/) const
{
    return new QTestLiteWindow(this, mPrimaryScreen, widget);
}



QPixmap QTestLiteIntegration::grabWindow(WId window, int x, int y, int width, int height) const
{
    QImage img = xd->grabWindow(window, x, y, width, height);
    return QPixmap::fromImage(img);
}

#ifndef QT_NO_OPENGL
bool QTestLiteIntegration::hasOpenGL() const
{
    return glXQueryExtension(xd->display, 0, 0) != 0;
}

QPlatformGLContext * QTestLiteIntegration::createGLContext()
{
    return new QGLXGLContext(xd->display);
}

#endif // QT_NO_OPENGL


QT_END_NAMESPACE
