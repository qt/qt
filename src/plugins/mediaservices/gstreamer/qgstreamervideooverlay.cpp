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

#include "qgstreamervideooverlay.h"
#include "qvideosurfacegstsink.h"

#include <QtMultimedia/qvideosurfaceformat.h>

#include "qx11videosurface.h"

QT_BEGIN_NAMESPACE

QGstreamerVideoOverlay::QGstreamerVideoOverlay(QObject *parent)
    : QVideoWindowControl(parent)
    , m_surface(new QX11VideoSurface)
    , m_videoSink(reinterpret_cast<GstElement*>(QVideoSurfaceGstSink::createSink(m_surface)))
    , m_aspectRatioMode(Qt::KeepAspectRatio)
    , m_fullScreen(false)
{
    if (m_videoSink) {
        gst_object_ref(GST_OBJECT(m_videoSink)); //Take ownership
        gst_object_sink(GST_OBJECT(m_videoSink));
    }

    connect(m_surface, SIGNAL(surfaceFormatChanged(QVideoSurfaceFormat)),
            this, SLOT(surfaceFormatChanged()));
}

QGstreamerVideoOverlay::~QGstreamerVideoOverlay()
{
    if (m_videoSink)
        gst_object_unref(GST_OBJECT(m_videoSink));

    delete m_surface;
}

WId QGstreamerVideoOverlay::winId() const
{
    return m_surface->winId();
}

void QGstreamerVideoOverlay::setWinId(WId id)
{
    m_surface->setWinId(id);
}

QRect QGstreamerVideoOverlay::displayRect() const
{
    return m_displayRect;
}

void QGstreamerVideoOverlay::setDisplayRect(const QRect &rect)
{
    m_displayRect = rect;

    setScaledDisplayRect();
}

Qt::AspectRatioMode QGstreamerVideoOverlay::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void QGstreamerVideoOverlay::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    m_aspectRatioMode = mode;

    setScaledDisplayRect();
}

void QGstreamerVideoOverlay::repaint()
{
}

int QGstreamerVideoOverlay::brightness() const
{
    return m_surface->brightness();
}

void QGstreamerVideoOverlay::setBrightness(int brightness)
{
    m_surface->setBrightness(brightness);

    emit brightnessChanged(m_surface->brightness());
}

int QGstreamerVideoOverlay::contrast() const
{
    return m_surface->contrast();
}

void QGstreamerVideoOverlay::setContrast(int contrast)
{
    m_surface->setContrast(contrast);

    emit contrastChanged(m_surface->contrast());
}

int QGstreamerVideoOverlay::hue() const
{
    return m_surface->hue();
}

void QGstreamerVideoOverlay::setHue(int hue)
{
    m_surface->setHue(hue);

    emit hueChanged(m_surface->hue());
}

int QGstreamerVideoOverlay::saturation() const
{
    return m_surface->saturation();
}

void QGstreamerVideoOverlay::setSaturation(int saturation)
{
    m_surface->setSaturation(saturation);

    emit saturationChanged(m_surface->saturation());
}

bool QGstreamerVideoOverlay::isFullScreen() const
{
    return m_fullScreen;
}

void QGstreamerVideoOverlay::setFullScreen(bool fullScreen)
{
    emit fullScreenChanged(m_fullScreen = fullScreen);
}

QSize QGstreamerVideoOverlay::nativeSize() const
{
    return m_surface->surfaceFormat().sizeHint();
}

QAbstractVideoSurface *QGstreamerVideoOverlay::surface() const
{
    return m_surface;
}

GstElement *QGstreamerVideoOverlay::videoSink()
{
    return m_videoSink;
}

void QGstreamerVideoOverlay::surfaceFormatChanged()
{
    setScaledDisplayRect();

    emit nativeSizeChanged();
}

void QGstreamerVideoOverlay::setScaledDisplayRect()
{
    QRect formatViewport = m_surface->surfaceFormat().viewport();

    switch (m_aspectRatioMode) {
    case Qt::KeepAspectRatio:
        {
            QSize size = m_surface->surfaceFormat().sizeHint();
            size.scale(m_displayRect.size(), Qt::KeepAspectRatio);

            QRect rect(QPoint(0, 0), size);
            rect.moveCenter(m_displayRect.center());

            m_surface->setDisplayRect(rect);
            m_surface->setViewport(formatViewport);
        }
        break;
    case Qt::IgnoreAspectRatio:
        m_surface->setDisplayRect(m_displayRect);
        m_surface->setViewport(formatViewport);
        break;
    case Qt::KeepAspectRatioByExpanding:
        {
            QSize size = m_displayRect.size();
            size.scale(m_surface->surfaceFormat().sizeHint(), Qt::KeepAspectRatio);

            QRect viewport(QPoint(0, 0), size);
            viewport.moveCenter(formatViewport.center());

            m_surface->setDisplayRect(m_displayRect);
            m_surface->setViewport(viewport);
        }
        break;
    };
}

QT_END_NAMESPACE

