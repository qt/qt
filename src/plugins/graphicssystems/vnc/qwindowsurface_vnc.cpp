/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
    mImage = QImage(window->size(), mScreen->format());
}

QVNCWindowSurface::~QVNCWindowSurface()
{
    mScreen->removeWindowSurface(this);
}

QPaintDevice *QVNCWindowSurface::paintDevice()
{
    return &mImage;
}

void QVNCWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget);
    Q_UNUSED(offset);

    QRect currentGeometry = geometry();
    // If this is a move, redraw the previous location
    if (oldGeometry != currentGeometry) {
        mScreen->setDirty(oldGeometry);
        oldGeometry = currentGeometry;
    }

    QRect dirtyClient = region.boundingRect();
    QRect dirtyRegion(currentGeometry.left() + dirtyClient.left(),
                      currentGeometry.top() + dirtyClient.top(),
                      dirtyClient.width(),
                      dirtyClient.height());
    mScreen->setDirty(dirtyRegion);
}

void QVNCWindowSurface::setGeometry(const QRect &rect)
{
    // store previous geometry for screen update
    oldGeometry = geometry();

    // change the widget's QImage if this is a resize
    if (mImage.size() != rect.size())
        mImage = QImage(rect.size(), mScreen->format());

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
