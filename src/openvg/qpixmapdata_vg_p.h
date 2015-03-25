/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpenVG module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPIXMAPDATA_VG_P_H
#define QPIXMAPDATA_VG_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtGui/private/qpixmap_raster_p.h>
#include <QtGui/private/qvolatileimage_p.h>
#include "qvg_p.h"

#if defined(Q_OS_SYMBIAN)
class RSGImage;
#endif

QT_BEGIN_NAMESPACE

class QEglContext;
class QVGImagePool;
class QImageReader;

#if !defined(QT_NO_EGL)
class QVGPixmapData;
class QVGSharedContext;

void qt_vg_register_pixmap(QVGPixmapData *pd);
void qt_vg_unregister_pixmap(QVGPixmapData *pd);
void qt_vg_hibernate_pixmaps(QVGSharedContext *context);
#endif

class QNativeImageHandleProvider;

class Q_OPENVG_EXPORT QVGPixmapData : public QPixmapData
{
public:
    QVGPixmapData(PixelType type);
    ~QVGPixmapData();

    QPixmapData *createCompatiblePixmapData() const;

    // Is this pixmap valid (i.e. non-zero in size)?
    bool isValid() const;

    void resize(int width, int height);
    void fromImage(const QImage &image, Qt::ImageConversionFlags flags);
    void fromImageReader(QImageReader *imageReader,
                          Qt::ImageConversionFlags flags);
    bool fromFile(const QString &filename, const char *format,
                          Qt::ImageConversionFlags flags);
    bool fromData(const uchar *buffer, uint len, const char *format,
                          Qt::ImageConversionFlags flags);

    void fill(const QColor &color);
    bool hasAlphaChannel() const;
    void setAlphaChannel(const QPixmap &alphaChannel);
    QImage toImage() const;
    void copy(const QPixmapData *data, const QRect &rect);
    QImage *buffer();
    QPaintEngine* paintEngine() const;

    // Return the VGImage form of this pixmap, creating it if necessary.
    // This assumes that there is a VG context current.
    virtual VGImage toVGImage();

    // Return the VGImage form for a specific opacity setting.
    virtual VGImage toVGImage(qreal opacity);

    // Detach this image from the image pool.
    virtual void detachImageFromPool();

    // Release the VG resources associated with this pixmap and copy
    // the pixmap's contents out of the GPU back into main memory.
    // The VG resource will be automatically recreated the next time
    // toVGImage() is called.  Does nothing if the pixmap cannot be
    // hibernated for some reason (e.g. VGImage is shared with another
    // process via a SgImage).
    virtual void hibernate();

    // Called when the QVGImagePool wants to reclaim this pixmap's
    // VGImage objects to reuse storage.
    virtual void reclaimImages();

    // If vgImage is valid but source is null, copies pixel data from GPU back
    // into main memory and destroys vgImage. For a normal pixmap this function
    // does nothing, however if the pixmap was created directly from a VGImage
    // (e.g. via SgImage on Symbian) then by doing the readback this ensures
    // that QImage-based functions can operate too.
    virtual void ensureReadback(bool readOnly) const;

    QSize size() const { return QSize(w, h); }

#if defined(Q_OS_SYMBIAN)
    QVolatileImage toVolatileImage() const;
    void* toNativeType(NativeType type);
    void fromNativeType(void* pixmap, NativeType type);
    bool initFromNativeImageHandle(void *handle, const QString &type);
    void createFromNativeImageHandleProvider();
    void releaseNativeImageHandle();
#endif

protected:
    int metric(QPaintDevice::PaintDeviceMetric metric) const;
    void createPixmapForImage(QImage &image, Qt::ImageConversionFlags flags, bool inPlace);

#if defined(Q_OS_SYMBIAN)
    void cleanup();
#endif

private:
    QVGPixmapData *nextLRU;
    QVGPixmapData *prevLRU;
    bool inLRU;
    friend class QVGImagePool;
    friend class QVGPaintEngine;

#if !defined(QT_NO_EGL)
    QVGPixmapData *next;
    QVGPixmapData *prev;

    friend void qt_vg_register_pixmap(QVGPixmapData *pd);
    friend void qt_vg_unregister_pixmap(QVGPixmapData *pd);
    friend void qt_vg_hibernate_pixmaps(QVGSharedContext *context);
#endif

protected:
    QSize prevSize;
    VGImage vgImage;
    VGImage vgImageOpacity;
    qreal cachedOpacity;
    mutable QVolatileImage source;
    mutable bool recreate;
    bool inImagePool;
#if !defined(QT_NO_EGL)
    mutable QEglContext *context;
#endif

#if defined(Q_OS_SYMBIAN)
    mutable QNativeImageHandleProvider *nativeImageHandleProvider;
    void *nativeImageHandle;
    QString nativeImageType;
#endif

    void forceToImage(bool allowReadback = true);
    QImage::Format sourceFormat() const;
    QImage::Format idealFormat(QImage *image, Qt::ImageConversionFlags flags) const;
    void updateSerial();

    void destroyImageAndContext();
    void destroyImages();
};

QT_END_NAMESPACE

#endif
