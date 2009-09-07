/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenVG module of the Qt Toolkit.
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
** In addition, as a special exception, Nokia gives you certain
** additional rights.  These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
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


#include "qwindowsurface_vnc.h"
#include "qgraphicssystem_vnc.h"
#include <QtCore/qdebug.h>
#include <QtGui/qpainter.h>
#include <private/qapplication_p.h>

QT_BEGIN_NAMESPACE

QVNCWindowSurface::QVNCWindowSurface(QVNCGraphicsSystem *graphicsSystem,
                                     QVNCGraphicsSystemScreen *screen, QWidget *window)
    : QWindowSurface(window),
      mScreen(screen)
{
}

QVNCWindowSurface::~QVNCWindowSurface()
{
}

QPaintDevice *QVNCWindowSurface::paintDevice()
{
    return mScreen->mScreenImage;
}

void QVNCWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget);
    Q_UNUSED(offset);

    QRect rect = geometry();
    QPoint topLeft = rect.topLeft();

    mScreen->setDirty(region.boundingRect());
    // server->flush(region);

}

void QVNCWindowSurface::setGeometry(const QRect &)
{

// any size you like as long as it's full-screen...

    QRect rect(mScreen->availableGeometry());
    QApplicationPrivate::handleGeometryChange(this->window(), rect);

    QWindowSurface::setGeometry(rect);
}

bool QVNCWindowSurface::scroll(const QRegion &area, int dx, int dy)
{
    return QWindowSurface::scroll(area, dx, dy);
}

void QVNCWindowSurface::beginPaint(const QRegion &region)
{
    Q_UNUSED(region);
}

void QVNCWindowSurface::endPaint(const QRegion &region)
{
    Q_UNUSED(region);
}

QT_END_NAMESPACE
