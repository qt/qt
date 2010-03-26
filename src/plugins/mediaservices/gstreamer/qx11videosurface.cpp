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

#include <QtCore/qvariant.h>
#include <QtCore/qdebug.h>
#include <QtGui/qx11info_x11.h>
#include <QtMultimedia/qvideosurfaceformat.h>

#include "qx11videosurface.h"

Q_DECLARE_METATYPE(::XvImage*);

QT_BEGIN_NAMESPACE

static QAbstractVideoBuffer::HandleType XvHandleType = QAbstractVideoBuffer::HandleType(4);

struct XvFormatRgb
{
    QVideoFrame::PixelFormat pixelFormat;
    int bits_per_pixel;
    int format;
    int num_planes;

    int depth;
    unsigned int red_mask;
    unsigned int green_mask;
    unsigned int blue_mask;

};

bool operator ==(const XvImageFormatValues &format, const XvFormatRgb &rgb)
{
    return format.type == XvRGB
            && format.bits_per_pixel == rgb.bits_per_pixel
            && format.format         == rgb.format
            && format.num_planes     == rgb.num_planes
            && format.depth          == rgb.depth
            && format.red_mask       == rgb.red_mask
            && format.blue_mask      == rgb.blue_mask;
}

static const XvFormatRgb qt_xvRgbLookup[] =
{
    { QVideoFrame::Format_ARGB32, 32, XvPacked, 1, 32, 0x00FF0000, 0x0000FF00, 0x000000FF },
    { QVideoFrame::Format_RGB32 , 32, XvPacked, 1, 24, 0x00FF0000, 0x0000FF00, 0x000000FF },
    { QVideoFrame::Format_RGB24 , 24, XvPacked, 1, 24, 0x00FF0000, 0x0000FF00, 0x000000FF },
    { QVideoFrame::Format_RGB565, 16, XvPacked, 1, 16, 0x0000F800, 0x000007E0, 0x0000001F },
    { QVideoFrame::Format_BGRA32, 32, XvPacked, 1, 32, 0xFF000000, 0x00FF0000, 0x0000FF00 },
    { QVideoFrame::Format_BGR32 , 32, XvPacked, 1, 24, 0x00FF0000, 0x0000FF00, 0x000000FF },
    { QVideoFrame::Format_BGR24 , 24, XvPacked, 1, 24, 0x00FF0000, 0x0000FF00, 0x000000FF },
    { QVideoFrame::Format_BGR565, 16, XvPacked, 1, 16, 0x0000F800, 0x000007E0, 0x0000001F }
};

struct XvFormatYuv
{
    QVideoFrame::PixelFormat pixelFormat;
    int bits_per_pixel;
    int format;
    int num_planes;

    unsigned int y_sample_bits;
    unsigned int u_sample_bits;
    unsigned int v_sample_bits;
    unsigned int horz_y_period;
    unsigned int horz_u_period;
    unsigned int horz_v_period;
    unsigned int vert_y_period;
    unsigned int vert_u_period;
    unsigned int vert_v_period;
    char component_order[32];
};

bool operator ==(const XvImageFormatValues &format, const XvFormatYuv &yuv)
{
    return format.type == XvYUV
            && format.bits_per_pixel == yuv.bits_per_pixel
            && format.format         == yuv.format
            && format.num_planes     == yuv.num_planes
            && format.y_sample_bits  == yuv.y_sample_bits
            && format.u_sample_bits  == yuv.u_sample_bits
            && format.v_sample_bits  == yuv.v_sample_bits
            && format.horz_y_period  == yuv.horz_y_period
            && format.horz_u_period  == yuv.horz_u_period
            && format.horz_v_period  == yuv.horz_v_period
            && format.horz_y_period  == yuv.vert_y_period
            && format.vert_u_period  == yuv.vert_u_period
            && format.vert_v_period  == yuv.vert_v_period
            && qstrncmp(format.component_order, yuv.component_order, 32) == 0;
}

static const XvFormatYuv qt_xvYuvLookup[] =
{
    { QVideoFrame::Format_YUV444 , 24, XvPacked, 1, 8, 8, 8, 1, 1, 1, 1, 1, 1, "YUV"  },
    { QVideoFrame::Format_YUV420P, 12, XvPlanar, 3, 8, 8, 8, 1, 2, 2, 1, 2, 2, "YUV"  },
    { QVideoFrame::Format_YV12   , 12, XvPlanar, 3, 8, 8, 8, 1, 2, 2, 1, 2, 2, "YVU"  },
    { QVideoFrame::Format_UYVY   , 16, XvPacked, 1, 8, 8, 8, 1, 2, 2, 1, 1, 1, "UYVY" },
    { QVideoFrame::Format_YUYV   , 16, XvPacked, 1, 8, 8, 8, 1, 2, 2, 1, 1, 1, "YUY2" },
    { QVideoFrame::Format_YUYV   , 16, XvPacked, 1, 8, 8, 8, 1, 2, 2, 1, 1, 1, "YUYV" },
    { QVideoFrame::Format_NV12   , 12, XvPlanar, 2, 8, 8, 8, 1, 2, 2, 1, 2, 2, "YUV"  },
    { QVideoFrame::Format_NV12   , 12, XvPlanar, 2, 8, 8, 8, 1, 2, 2, 1, 2, 2, "YVU"  },
    { QVideoFrame::Format_Y8     , 8 , XvPlanar, 1, 8, 0, 0, 1, 0, 0, 1, 0, 0, "Y"    }
};

QX11VideoSurface::QX11VideoSurface(QObject *parent)
    : QAbstractVideoSurface(parent)
    , m_winId(0)
    , m_portId(0)
    , m_gc(0)
    , m_image(0)
{
}

QX11VideoSurface::~QX11VideoSurface()
{
    if (m_gc)
        XFreeGC(QX11Info::display(), m_gc);

    if (m_portId != 0)
        XvUngrabPort(QX11Info::display(), m_portId, 0);
}

WId QX11VideoSurface::winId() const
{
    return m_winId;
}

void QX11VideoSurface::setWinId(WId id)
{
    if (id == m_winId)
        return;

    if (m_image)
        XFree(m_image);

    if (m_gc) {
        XFreeGC(QX11Info::display(), m_gc);
        m_gc = 0;
    }

    if (m_portId != 0)
        XvUngrabPort(QX11Info::display(), m_portId, 0);

    m_supportedPixelFormats.clear();
    m_formatIds.clear();

    m_winId = id;

    if (m_winId && findPort()) {
        querySupportedFormats();

        m_gc = XCreateGC(QX11Info::display(), m_winId, 0, 0);

        if (m_image) {
            m_image = 0;

            if (!start(surfaceFormat()))
                QAbstractVideoSurface::stop();
        }
    } else if (m_image) {
        m_image = 0;

        QAbstractVideoSurface::stop();
    }

    emit supportedFormatsChanged();
}

QRect QX11VideoSurface::displayRect() const
{
    return m_displayRect;
}

void QX11VideoSurface::setDisplayRect(const QRect &rect)
{
    m_displayRect = rect;
}

QRect QX11VideoSurface::viewport() const
{
    return m_viewport;
}

void QX11VideoSurface::setViewport(const QRect &rect)
{
    m_viewport = rect;
}

int QX11VideoSurface::brightness() const
{
    return getAttribute("XV_BRIGHTNESS", m_brightnessRange.first, m_brightnessRange.second);
}

void QX11VideoSurface::setBrightness(int brightness)
{
    setAttribute("XV_BRIGHTNESS", brightness, m_brightnessRange.first, m_brightnessRange.second);
}

int QX11VideoSurface::contrast() const
{
    return getAttribute("XV_CONTRAST", m_contrastRange.first, m_contrastRange.second);
}

void QX11VideoSurface::setContrast(int contrast)
{
    setAttribute("XV_CONTRAST", contrast, m_contrastRange.first, m_contrastRange.second);
}

int QX11VideoSurface::hue() const
{
    return getAttribute("XV_HUE", m_hueRange.first, m_hueRange.second);
}

void QX11VideoSurface::setHue(int hue)
{
    setAttribute("XV_HUE", hue, m_hueRange.first, m_hueRange.second);
}

int QX11VideoSurface::saturation() const
{
    return getAttribute("XV_SATURATION", m_saturationRange.first, m_saturationRange.second);
}

void QX11VideoSurface::setSaturation(int saturation)
{
    setAttribute("XV_SATURATION", saturation, m_saturationRange.first, m_saturationRange.second);
}

int QX11VideoSurface::getAttribute(const char *attribute, int minimum, int maximum) const
{
    if (m_portId != 0) {
        Display *display = QX11Info::display();

        Atom atom = XInternAtom(display, attribute, True);

        int value = 0;

        XvGetPortAttribute(display, m_portId, atom, &value);

        return redistribute(value, minimum, maximum, -100, 100);
    } else {
        return 0;
    }
}

void QX11VideoSurface::setAttribute(const char *attribute, int value, int minimum, int maximum)
{
    if (m_portId != 0) {
        Display *display = QX11Info::display();

        Atom atom = XInternAtom(display, attribute, True);

        XvSetPortAttribute(
                display, m_portId, atom, redistribute(value, -100, 100, minimum, maximum));
    }
}

int QX11VideoSurface::redistribute(
        int value, int fromLower, int fromUpper, int toLower, int toUpper)
{
    return fromUpper != fromLower
            ? ((value - fromLower) * (toUpper - toLower) / (fromUpper - fromLower)) + toLower
            : 0;
}

QList<QVideoFrame::PixelFormat> QX11VideoSurface::supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType) const
{
    return handleType == QAbstractVideoBuffer::NoHandle || handleType ==  XvHandleType
            ? m_supportedPixelFormats
            : QList<QVideoFrame::PixelFormat>();
}

bool QX11VideoSurface::start(const QVideoSurfaceFormat &format)
{
    if (m_image)
        XFree(m_image);

    int xvFormatId = 0;
    for (int i = 0; i < m_supportedPixelFormats.count(); ++i) {
        if (m_supportedPixelFormats.at(i) == format.pixelFormat()) {
            xvFormatId = m_formatIds.at(i);
            break;
        }
    }

    if (xvFormatId == 0) {
        setError(UnsupportedFormatError);
    } else {
        XvImage *image = XvCreateImage(
                QX11Info::display(),
                m_portId,
                xvFormatId,
                0,
                format.frameWidth(),
                format.frameHeight());

        if (!image) {
            setError(ResourceError);
        } else {
            m_viewport = format.viewport();
            m_image = image;

            QVideoSurfaceFormat newFormat = format;
            newFormat.setProperty("portId", QVariant(quint64(m_portId)));
            newFormat.setProperty("xvFormatId", xvFormatId);
            newFormat.setProperty("dataSize", image->data_size);

            return QAbstractVideoSurface::start(newFormat);
        }
    }

    if (m_image) {
        m_image = 0;

        QAbstractVideoSurface::stop();
    }

    return false;
}

void QX11VideoSurface::stop()
{
    if (m_image) {
        XFree(m_image);
        m_image = 0;

        QAbstractVideoSurface::stop();
    }
}

bool QX11VideoSurface::present(const QVideoFrame &frame)
{
    if (!m_image) {
        setError(StoppedError);
        return false;
    } else if (m_image->width != frame.width() || m_image->height != frame.height()) {
        setError(IncorrectFormatError);
        return false;
    } else {
        QVideoFrame frameCopy(frame);

        if (!frameCopy.map(QAbstractVideoBuffer::ReadOnly)) {
            setError(IncorrectFormatError);
            return false;
        } else {
            bool presented = false;

            if (frame.handleType() != XvHandleType &&
                m_image->data_size > frame.mappedBytes()) {
                qWarning("Insufficient frame buffer size");
                setError(IncorrectFormatError);
            } else if (frame.handleType() != XvHandleType &&
                       m_image->num_planes > 0 &&
                       m_image->pitches[0] != frame.bytesPerLine()) {
                qWarning("Incompatible frame pitches");
                setError(IncorrectFormatError);
            } else {
                if (frame.handleType() != XvHandleType) {
                    m_image->data = reinterpret_cast<char *>(frameCopy.bits());

                    //qDebug() << "copy frame";
                    XvPutImage(
                            QX11Info::display(),
                            m_portId,
                            m_winId,
                            m_gc,
                            m_image,
                            m_viewport.x(),
                            m_viewport.y(),
                            m_viewport.width(),
                            m_viewport.height(),
                            m_displayRect.x(),
                            m_displayRect.y(),
                            m_displayRect.width(),
                            m_displayRect.height());

                    m_image->data = 0;
                } else {
                    XvImage *img = frame.handle().value<XvImage*>();

                    //qDebug() << "render directly";
                    if (img)
                        XvShmPutImage(
                           QX11Info::display(),
                           m_portId,
                           m_winId,
                           m_gc,
                           img,
                           m_viewport.x(),
                           m_viewport.y(),
                           m_viewport.width(),
                           m_viewport.height(),
                           m_displayRect.x(),
                           m_displayRect.y(),
                           m_displayRect.width(),
                           m_displayRect.height(),
                           false);
                }

                presented = true;
            }

            frameCopy.unmap();

            return presented;
        }
    }
}

bool QX11VideoSurface::findPort()
{
    unsigned int count = 0;
    XvAdaptorInfo *adaptors = 0;
    bool portFound = false;

    if (XvQueryAdaptors(QX11Info::display(), m_winId, &count, &adaptors) == Success) {
        for (unsigned int i = 0; i < count && !portFound; ++i) {
            if (adaptors[i].type & XvImageMask) {
                m_portId = adaptors[i].base_id;

                for (unsigned int j = 0; j < adaptors[i].num_ports && !portFound; ++j, ++m_portId)
                    portFound = XvGrabPort(QX11Info::display(), m_portId, 0) == Success;
            }
        }
        XvFreeAdaptorInfo(adaptors);
    }

    return portFound;
}

void QX11VideoSurface::querySupportedFormats()
{
    int count = 0;
    if (XvImageFormatValues *imageFormats = XvListImageFormats(
            QX11Info::display(), m_portId, &count)) {
        const int rgbCount = sizeof(qt_xvRgbLookup) / sizeof(XvFormatRgb);
        const int yuvCount = sizeof(qt_xvYuvLookup) / sizeof(XvFormatYuv);

        for (int i = 0; i < count; ++i) {
            switch (imageFormats[i].type) {
            case XvRGB:
                for (int j = 0; j < rgbCount; ++j) {
                    if (imageFormats[i] == qt_xvRgbLookup[j]) {
                        m_supportedPixelFormats.append(qt_xvRgbLookup[j].pixelFormat);
                        m_formatIds.append(imageFormats[i].id);
                        break;
                    }
                }
                break;
            case XvYUV:
                for (int j = 0; j < yuvCount; ++j) {
                    if (imageFormats[i] == qt_xvYuvLookup[j]) {
                        m_supportedPixelFormats.append(qt_xvYuvLookup[j].pixelFormat);
                        m_formatIds.append(imageFormats[i].id);
                        break;
                    }
                }
                break;
            }
        }
        XFree(imageFormats);
    }

    m_brightnessRange = qMakePair(0, 0);
    m_contrastRange = qMakePair(0, 0);
    m_hueRange = qMakePair(0, 0);
    m_saturationRange = qMakePair(0, 0);

    if (XvAttribute *attributes = XvQueryPortAttributes(QX11Info::display(), m_portId, &count)) {
        for (int i = 0; i < count; ++i) {
            if (qstrcmp(attributes[i].name, "XV_BRIGHTNESS") == 0)
                m_brightnessRange = qMakePair(attributes[i].min_value, attributes[i].max_value);
            else if (qstrcmp(attributes[i].name, "XV_CONTRAST") == 0)
                m_contrastRange = qMakePair(attributes[i].min_value, attributes[i].max_value);
            else if (qstrcmp(attributes[i].name, "XV_HUE") == 0)
                m_hueRange = qMakePair(attributes[i].min_value, attributes[i].max_value);
            else if (qstrcmp(attributes[i].name, "XV_SATURATION") == 0)
                m_saturationRange = qMakePair(attributes[i].min_value, attributes[i].max_value);
        }

        XFree(attributes);
    }
}

QT_END_NAMESPACE
