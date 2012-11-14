/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDIRECTFBPIXMAP_H
#define QDIRECTFBPIXMAP_H

#include <qglobal.h>

#ifndef QT_NO_QWS_DIRECTFB

#include <QtGui/private/qpixmapdata_p.h>
#include <QtGui/private/qpaintengine_raster_p.h>
#include "qdirectfbpaintdevice.h"
#include <directfb.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QDirectFBPaintEngine;

class QDirectFBPixmapData : public QPixmapData, public QDirectFBPaintDevice
{
public:
    QDirectFBPixmapData(QDirectFBScreen *screen, PixelType pixelType);
    ~QDirectFBPixmapData();

    // Re-implemented from QPixmapData:
    virtual void resize(int width, int height);
    virtual void fromImage(const QImage &image, Qt::ImageConversionFlags flags);
#ifdef QT_DIRECTFB_IMAGEPROVIDER
    virtual bool fromFile(const QString &filename, const char *format,
                          Qt::ImageConversionFlags flags);
    virtual bool fromData(const uchar *buffer, uint len, const char *format,
                          Qt::ImageConversionFlags flags);
#endif
    virtual void copy(const QPixmapData *data, const QRect &rect);
    virtual void fill(const QColor &color);
    virtual QPixmap transformed(const QTransform &matrix,
                                Qt::TransformationMode mode) const;
    virtual QImage toImage() const;
    virtual QPaintEngine *paintEngine() const;
    virtual QImage *buffer();
    virtual bool scroll(int dx, int dy, const QRect &rect);
    // Pure virtual in QPixmapData, so re-implement here and delegate to QDirectFBPaintDevice
    virtual int metric(QPaintDevice::PaintDeviceMetric m) const { return QDirectFBPaintDevice::metric(m); }

    inline QImage::Format pixelFormat() const { return imageFormat; }
    inline bool hasAlphaChannel() const { return alpha; }
    static bool hasAlphaChannel(const QImage &img, Qt::ImageConversionFlags flags = Qt::AutoColor);
private:
#ifdef QT_DIRECTFB_IMAGEPROVIDER
    bool fromDataBufferDescription(const DFBDataBufferDescription &dataBuffer);
#endif
    void invalidate();
    bool alpha;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_QWS_DIRECTFB

#endif // QDIRECTFBPIXMAP_H
