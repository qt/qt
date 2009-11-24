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

#include "qgraphicssystem_testlite.h"
#include "qwindowsurface_testlite.h"
#include <QtGui/private/qpixmap_raster_p.h>
#include <QtCore/qdebug.h>

#include <qgraphicssystemcursor.h>


#include "x11util.h"

QT_BEGIN_NAMESPACE

class MyCursor : QGraphicsSystemCursor
{
public:
    MyCursor(QGraphicsSystemScreen *screen) : QGraphicsSystemCursor(screen) {}

        // input methods
    void setCursor(const uchar */*data*/, const uchar */*mask*/, int width, int height, int hotX, int hotY) {qDebug() << "setCursor data..." << width << height << hotX << hotY;}

    void setCursor(Qt::CursorShape shape) {
        static int oldshape = -1;
        if (shape != oldshape) {
            qDebug() << "setCursor" << shape; QGraphicsSystemCursor::setCursor(shape);
            oldshape = shape;
        }
    }

    void changeCursor(QWidget * widget) {

        QTestLiteWindowSurface *ws = 0;
        if (widget) {
            QWidget *window = widget->window();
            ws = static_cast<QTestLiteWindowSurface*>(window->windowSurface());
        }

        //qDebug() << "changeCursor" << widget << ws;
        if (!ws)
            return;

        ws->setCursor(widget->cursor().shape());
    }

//     void changeCursor(QCursor * widgetCursor) {
//         //qDebug() << "changeCursor widgetCursor";
//         QGraphicsSystemCursor::changeCursor(widgetCursor);
//     }

    //#### remove this
    void pointerEvent(const QMouseEvent & event) {
        Q_UNUSED(event);
#if 0
        qDebug() << "pointerEvent" << event.globalPos();
#endif
    }
};


QTestLiteGraphicsSystem::QTestLiteGraphicsSystem()
{

    xd = new MyDisplay;

    mPrimaryScreen = new QTestLiteGraphicsSystemScreen();

    mPrimaryScreen->mGeometry = QRect
        (0, 0, xd->width, xd->height);
    mPrimaryScreen->mDepth = 32;
    mPrimaryScreen->mFormat = QImage::Format_RGB32;
    mPrimaryScreen->mPhysicalSize =
        QSize(xd->physicalWidth, xd->physicalHeight);

    mScreens.append(mPrimaryScreen);


    (void)new MyCursor(mPrimaryScreen);

}

QPixmapData *QTestLiteGraphicsSystem::createPixmapData(QPixmapData::PixelType type) const
{
    return new QRasterPixmapData(type);
}

QWindowSurface *QTestLiteGraphicsSystem::createWindowSurface(QWidget *widget) const
{
    if (widget->windowType() == Qt::Desktop)
        return 0;   // Don't create an explicit window surface for the destkop.
    return new QTestLiteWindowSurface
        (const_cast<QTestLiteGraphicsSystem *>(this), mPrimaryScreen, widget);
}


QPixmap QTestLiteGraphicsSystem::grabWindow(WId window, int x, int y, int width, int height) const
{
    qDebug() << "grabWindow" << hex << window << dec<< x << y << width << height;
    return QPixmap();
}



QT_END_NAMESPACE
