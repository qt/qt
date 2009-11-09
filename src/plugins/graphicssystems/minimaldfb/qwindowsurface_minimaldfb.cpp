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

#include "qwindowsurface_minimaldfb.h"
#include "qgraphicssystem_minimaldfb.h"
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

QDirectFbWindowSurface::QDirectFbWindowSurface
        (QDirectFbGraphicsSystemScreen *screen, QWidget *window)
    : QWindowSurface(window), m_screen(screen), m_lock(false)
{
    window->setWindowSurface(this);
    m_dfbWindow = m_screen->createWindow(window->rect());
    DFBResult result = m_dfbWindow->GetSurface(m_dfbWindow,&m_dfbSurface);
    if (result != DFB_OK) {
        DirectFBError("QDirectFbWindowSurface::QDirectFbWindowSurface: unable to get windows surface",result);
    }
}

QDirectFbWindowSurface::~QDirectFbWindowSurface()
{
}

QPaintDevice *QDirectFbWindowSurface::paintDevice()
{
    return m_image;
}

void QDirectFbWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget);
    Q_UNUSED(offset);

    const quint8 windowOpacity = quint8(widget->windowOpacity() * 0xff);
    m_dfbWindow->SetOpacity(m_dfbWindow,windowOpacity);

    QVector<QRect> rects = region.rects();
    for (int i = 0 ; i < rects.size(); i++) {
        const QRect rect = rects.at(i);
        DFBRegion dfbReg = { rect.x() + offset.x(),rect.y() + offset.y(),rect.right() + offset.x(),rect.bottom() + offset.y()};
        m_dfbSurface->Flip(m_dfbSurface,&dfbReg,DSFLIP_BLIT);
    }
}

void QDirectFbWindowSurface::setGeometry(const QRect &rect)
{
//    qDebug() << "QDirectF.bWindowSurface::setGeometry:" << (long)this << rect;
    bool wasLocked = false;
    if (m_lock){
        m_dfbSurface->Unlock(m_dfbSurface);
        wasLocked = true;
    }
    m_dfbSurface->Release(m_dfbSurface);
    QWindowSurface::setGeometry(rect);
    m_dfbWindow->SetBounds(m_dfbWindow, rect.x(),rect.y(),
                           rect.width(), rect.height());
//    m_dfbWindow->Resize(m_dfbWindow,rect.width(),rect.height());
//    m_dfbWindow->MoveTo(m_dfbWindow,rect.x(),rect.y());
    DFBResult result = m_dfbWindow->GetSurface(m_dfbWindow,&m_dfbSurface);
    if (result != DFB_OK)
        qDebug() << "could not resurface";

    if (wasLocked)
        lockSurfaceToImage();
}

bool QDirectFbWindowSurface::scroll(const QRegion &area, int dx, int dy)
{
    return QWindowSurface::scroll(area, dx, dy);
}

void QDirectFbWindowSurface::beginPaint(const QRegion &region)
{
    Q_UNUSED(region);
    if (!m_lock)
        lockSurfaceToImage();
}

void QDirectFbWindowSurface::endPaint(const QRegion &region)
{
    Q_UNUSED(region);
    if (m_lock){
        m_dfbSurface->Unlock(m_dfbSurface);
        m_lock = false;
    }
}

void QDirectFbWindowSurface::lockSurfaceToImage()
{
    if (m_lock)
        return;
    m_lock = true;

    int w,h;
    m_dfbSurface->GetSize(m_dfbSurface,&w,&h);
    uchar *data;
    int bpl;
    DFBResult result = m_dfbSurface->Lock(m_dfbSurface,static_cast<DFBSurfaceLockFlags>(DSLF_READ|DSLF_WRITE),reinterpret_cast<void **>(&data),&bpl);
    if (!result == DFB_OK) {
        DirectFBError("QDirectFbWindowSurface::lockSurfaceToImage() failed to lock surface",result);
        return;
    }
    QImage::Format format = QDirectFbGraphicsSystem::imageFormatFromSurface(m_dfbSurface);
    m_image = new QImage(data,w,h,bpl,format);
}

QT_END_NAMESPACE
