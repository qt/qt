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

#include <QtMultimedia/qvideosurfaceformat.h>
#include "s60videooverlay.h"
#include "s60videosurface.h"

QT_BEGIN_NAMESPACE

S60VideoOverlay::S60VideoOverlay(QObject *parent)
    : QVideoWindowControl(parent)
    , m_surface(new S60VideoSurface)
    , m_aspectRatioMode(Qt::KeepAspectRatio)
    , m_fullScreen(false)
{
    connect(m_surface, SIGNAL(surfaceFormatChanged(QVideoSurfaceFormat)),
            this, SLOT(surfaceFormatChanged()));
}

S60VideoOverlay::~S60VideoOverlay()
{
    delete m_surface;
}

WId S60VideoOverlay::winId() const
{
    return m_surface->winId();
}

void S60VideoOverlay::setWinId(WId id)
{
    m_surface->setWinId(id);
}

QRect S60VideoOverlay::displayRect() const
{
    return m_displayRect;
}

void S60VideoOverlay::setDisplayRect(const QRect &rect)
{
    m_displayRect = rect;

    setScaledDisplayRect();
}

Qt::AspectRatioMode S60VideoOverlay::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void S60VideoOverlay::setAspectRatioMode(Qt::AspectRatioMode ratio)
{
    m_aspectRatioMode = ratio;

    setScaledDisplayRect();
}

QSize S60VideoOverlay::customAspectRatio() const
{
    return m_aspectRatio;
}

void S60VideoOverlay::setCustomAspectRatio(const QSize &customRatio)
{
    m_aspectRatio = customRatio;

    setScaledDisplayRect();
}

void S60VideoOverlay::repaint()
{
}

int S60VideoOverlay::brightness() const
{
    return m_surface->brightness();
}

void S60VideoOverlay::setBrightness(int brightness)
{
    m_surface->setBrightness(brightness);

    emit brightnessChanged(m_surface->brightness());
}

int S60VideoOverlay::contrast() const
{
    return m_surface->contrast();
}

void S60VideoOverlay::setContrast(int contrast)
{
    m_surface->setContrast(contrast);

    emit contrastChanged(m_surface->contrast());
}

int S60VideoOverlay::hue() const
{
    return m_surface->hue();
}

void S60VideoOverlay::setHue(int hue)
{
    m_surface->setHue(hue);

    emit hueChanged(m_surface->hue());
}

int S60VideoOverlay::saturation() const
{
    return m_surface->saturation();
}

void S60VideoOverlay::setSaturation(int saturation)
{
    m_surface->setSaturation(saturation);

    emit saturationChanged(m_surface->saturation());
}

bool S60VideoOverlay::isFullScreen() const
{
    return m_fullScreen;
}

void S60VideoOverlay::setFullScreen(bool fullScreen)
{
    emit fullScreenChanged(m_fullScreen = fullScreen);
}

QSize S60VideoOverlay::nativeSize() const
{
    return m_surface->surfaceFormat().sizeHint();
}

QAbstractVideoSurface *S60VideoOverlay::surface() const
{
    return m_surface;
}

void S60VideoOverlay::surfaceFormatChanged()
{
    setScaledDisplayRect();

    emit nativeSizeChanged();
}

void S60VideoOverlay::setScaledDisplayRect()
{
    switch (m_aspectRatioMode) {
    case Qt::KeepAspectRatio:
        {
            QSize size = m_surface->surfaceFormat().viewport().size();

            size.scale(m_displayRect.size(), Qt::KeepAspectRatio);

            QRect rect(QPoint(0, 0), size);
            rect.moveCenter(m_displayRect.center());

            m_surface->setDisplayRect(rect);
        }
        break;
    case Qt::IgnoreAspectRatio:
        m_surface->setDisplayRect(m_displayRect);
        break;
    };
}

QT_END_NAMESPACE
