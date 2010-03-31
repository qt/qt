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

#include "qwindowsurface_directfb.h"
#include "qplatformintegration_directfb.h"
#include "qblitter_directfb.h"
#include "qdirectfbconvenience.h"
#include <private/qpixmap_blitter_p.h>

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

QDirectFbWindowSurface::QDirectFbWindowSurface(QWidget *window)
    : QWindowSurface(window), m_pixmap(0), m_pmdata(0),
      m_dfbWindow(0), m_dfbSurface(0)
{
    window->setWindowSurface(this);

    IDirectFBDisplayLayer *layer = QDirectFbConvenience::dfbDisplayLayer();
    DFBDisplayLayerConfig layerConfig;
    layer->GetConfiguration(layer,&layerConfig);

    DFBWindowDescription description;
    memset(&description,0,sizeof(DFBWindowDescription));
    description.flags = DFBWindowDescriptionFlags(DWDESC_WIDTH|DWDESC_HEIGHT|DWDESC_POSX|DWDESC_POSY|DWDESC_SURFACE_CAPS
#if DIRECTFB_MINOR_VERSION >= 1
                                                  |DWDESC_OPTIONS
#endif
                                                  |DWDESC_CAPS);
    description.width = window->rect().width();
    description.height = window->rect().height();
    description.posx = window->rect().x();
    description.posy = window->rect().y();

    if (layerConfig.surface_caps & DSCAPS_PREMULTIPLIED)
        description.surface_caps = DSCAPS_PREMULTIPLIED;
    description.pixelformat = layerConfig.pixelformat;

#if DIRECTFB_MINOR_VERSION >= 1
    description.options = DFBWindowOptions(DWOP_ALPHACHANNEL);
#endif
    description.caps = DFBWindowCapabilities(DWCAPS_DOUBLEBUFFER|DWCAPS_ALPHACHANNEL);
    description.surface_caps = DSCAPS_PREMULTIPLIED;

    DFBResult result = layer->CreateWindow(layer,&description,&m_dfbWindow);
    if (result != DFB_OK) {
        DirectFBError("QDirectFbGraphicsSystemScreen: failed to create window",result);
    }

    DFBWindowID id;
    m_dfbWindow->GetID(m_dfbWindow, &id);
    QDirectFbInput::instance()->addWindow(id,window);

    m_dfbWindow->GetSurface(m_dfbWindow,&m_dfbSurface);

    QDirectFbBlitter *blitter = new QDirectFbBlitter(window->rect().size(), m_dfbSurface);
    m_pmdata = new QBlittablePixmapData(QPixmapData::PixmapType);
    m_pmdata->setBlittable(blitter);
    m_pixmap = new QPixmap(m_pmdata);
}

QDirectFbWindowSurface::~QDirectFbWindowSurface()
{
    QDirectFbInput::instance()->removeWindow(this->window());
    m_dfbWindow->Destroy(m_dfbWindow);
}

QPaintDevice *QDirectFbWindowSurface::paintDevice()
{
    return m_pixmap;
}

void QDirectFbWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    m_pmdata->blittable()->unlock();

    const quint8 windowOpacity = quint8(widget->windowOpacity() * 0xff);
    m_dfbWindow->SetOpacity(m_dfbWindow,windowOpacity);

    QVector<QRect> rects = region.rects();
    for (int i = 0 ; i < rects.size(); i++) {
        const QRect rect = rects.at(i);
        DFBRegion dfbReg = { rect.x() + offset.x(),rect.y() + offset.y(),rect.right() + offset.x(),rect.bottom() + offset.y()};
        m_dfbSurface->Flip(m_dfbSurface, &dfbReg, DFBSurfaceFlipFlags(DSFLIP_BLIT|DSFLIP_ONSYNC));
    }
}

void QDirectFbWindowSurface::setGeometry(const QRect &rect)
{
    m_pmdata->blittable()->unlock();

    QWindowSurface::setGeometry(rect);
    m_dfbWindow->SetBounds(m_dfbWindow, rect.x(),rect.y(),
                           rect.width(), rect.height());

    //Have to add 1 ref ass it will be removed by deleting the old blitter in setBlittable
    m_dfbSurface->AddRef(m_dfbSurface);
    QDirectFbBlitter *blitter = new QDirectFbBlitter(rect.size(),m_dfbSurface);
    m_pmdata->setBlittable(blitter);
}

static inline void scrollSurface(IDirectFBSurface *surface, const QRect &r, int dx, int dy)
{
    const DFBRectangle rect = { r.x(), r.y(), r.width(), r.height() };
    surface->Blit(surface, surface, &rect, r.x() + dx, r.y() + dy);
    const DFBRegion region = { rect.x + dx, rect.y + dy, r.right() + dx, r.bottom() + dy };
    surface->Flip(surface, &region, DFBSurfaceFlipFlags(DSFLIP_BLIT));
}

bool QDirectFbWindowSurface::scroll(const QRegion &area, int dx, int dy)
{
    m_pmdata->blittable()->unlock();

    if (!m_dfbSurface || area.isEmpty())
        return false;
    m_dfbSurface->SetBlittingFlags(m_dfbSurface, DSBLIT_NOFX);
    if (area.rectCount() == 1) {
        scrollSurface(m_dfbSurface, area.boundingRect(), dx, dy);
    } else {
        const QVector<QRect> rects = area.rects();
        const int n = rects.size();
        for (int i=0; i<n; ++i) {
            scrollSurface(m_dfbSurface, rects.at(i), dx, dy);
        }
    }
    return true;
}

void QDirectFbWindowSurface::beginPaint(const QRegion &region)
{
    Q_UNUSED(region);
}

void QDirectFbWindowSurface::endPaint(const QRegion &region)
{
    Q_UNUSED(region);
}

void QDirectFbWindowSurface::setVisible(bool visible)
{
    m_pmdata->blittable()->unlock();

    if (visible) {
        int x = this->geometry().x();
        int y = this->geometry().y();
        m_dfbWindow->MoveTo(m_dfbWindow,x,y);
    } else {
        IDirectFBDisplayLayer *displayLayer;
        QDirectFbConvenience::dfbInterface()->GetDisplayLayer(QDirectFbConvenience::dfbInterface(),DLID_PRIMARY,&displayLayer);

        DFBDisplayLayerConfig config;
        displayLayer->GetConfiguration(displayLayer,&config);
        m_dfbWindow->MoveTo(m_dfbWindow,config.width+1,config.height + 1);
    }
}

Qt::WindowFlags QDirectFbWindowSurface::setWindowFlags(Qt::WindowFlags flags)
{
    switch (flags & Qt::WindowType_Mask) {
    case Qt::ToolTip: {
        DFBWindowOptions options;
        m_dfbWindow->GetOptions(m_dfbWindow,&options);
        options = DFBWindowOptions(options | DWOP_GHOST);
        m_dfbWindow->SetOptions(m_dfbWindow,options);
        break; }
    default:
        break;
    }

    m_dfbWindow->SetStackingClass(m_dfbWindow, flags & Qt::WindowStaysOnTopHint ? DWSC_UPPER : DWSC_MIDDLE);
    return flags;
}

void QDirectFbWindowSurface::raise()
{
    m_dfbWindow->RaiseToTop(m_dfbWindow);
}

void QDirectFbWindowSurface::lower()
{
    m_dfbWindow->LowerToBottom(m_dfbWindow);
}

WId QDirectFbWindowSurface::winId() const
{
    DFBWindowID id;
    m_dfbWindow->GetID(m_dfbWindow, &id);
    return WId(id);
}


QT_END_NAMESPACE
