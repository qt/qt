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
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
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
#include <private/qvg_p.h>
#if !defined(QT_NO_EGL)
#include <QtGui/private/qegl_p.h>
#endif

QT_BEGIN_NAMESPACE

class QEglContext;

class Q_OPENVG_EXPORT QVGPixmapData : public QPixmapData
{
public:
    QVGPixmapData(PixelType type);
    ~QVGPixmapData();

    // Is this pixmap valid (i.e. non-zero in size)?
    bool isValid() const;

    void resize(int width, int height);
    void fromImage(const QImage &image, Qt::ImageConversionFlags flags);

    void fill(const QColor &color);
    bool hasAlphaChannel() const;
    void setAlphaChannel(const QPixmap &alphaChannel);
    QImage toImage() const;
    QImage *buffer();
    QPaintEngine* paintEngine() const;

    // Return the VGImage form of this pixmap, creating it if necessary.
    // This assumes that there is a VG context current.
    VGImage toVGImage();

    // Return the VGImage form for a specific opacity setting.
    VGImage toVGImage(qreal opacity);

    QSize size() const { return QSize(w, h); }

protected:
    int metric(QPaintDevice::PaintDeviceMetric metric) const;

private:
    VGImage vgImage;
    VGImage vgImageOpacity;
    qreal cachedOpacity;
    mutable QImage source;
    mutable bool recreate;
#if !defined(QT_NO_EGL)
    mutable QEglContext *context;
#endif

    void forceToImage();
    QImage::Format sourceFormat() const;
};

QT_END_NAMESPACE

#endif
