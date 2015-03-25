/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion <blackberry-qt@qnx.com>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
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

//#define QBBBUFFER_DEBUG


#include "qbbbuffer.h"

#include <QDebug>

#include <errno.h>
#include <sys/mman.h>

QT_BEGIN_NAMESPACE

QBBBuffer::QBBBuffer()
    : mBuffer(NULL)
{
#if defined(QBBBUFFER_DEBUG)
    qDebug() << "QBBBuffer::QBBBuffer - empty";
#endif
}

QBBBuffer::QBBBuffer(screen_buffer_t buffer)
    : mBuffer(buffer)
{
#if defined(QBBBUFFER_DEBUG)
    qDebug() << "QBBBuffer::QBBBuffer - normal";
#endif

    // get size of buffer
    errno = 0;
    int size[2];
    int result = screen_get_buffer_property_iv(buffer, SCREEN_PROPERTY_BUFFER_SIZE, size);
    if (result != 0) {
        qFatal("QBB: failed to query buffer size, errno=%d", errno);
    }

    // get stride of buffer
    errno = 0;
    int stride;
    result = screen_get_buffer_property_iv(buffer, SCREEN_PROPERTY_STRIDE, &stride);
    if (result != 0) {
        qFatal("QBB: failed to query buffer stride, errno=%d", errno);
    }

    // get access to buffer's data
    errno = 0;
    uchar* dataPtr = NULL;
    result = screen_get_buffer_property_pv(buffer, SCREEN_PROPERTY_POINTER, (void **)&dataPtr);
    if (result != 0) {
        qFatal("QBB: failed to query buffer pointer, errno=%d", errno);
    }
    if (dataPtr == NULL) {
        qFatal("QBB: buffer pointer is NULL, errno=%d", errno);
    }

    // get format of buffer
    errno = 0;
    int screenFormat;
    result = screen_get_buffer_property_iv(buffer, SCREEN_PROPERTY_FORMAT, &screenFormat);
    if (result != 0) {
        qFatal("QBB: failed to query buffer format, errno=%d", errno);
    }

    // convert screen format to QImage format
    QImage::Format imageFormat = QImage::Format_Invalid;
    switch (screenFormat) {
    case SCREEN_FORMAT_RGBX4444:
        imageFormat = QImage::Format_RGB444;
        break;
    case SCREEN_FORMAT_RGBA4444:
        imageFormat = QImage::Format_ARGB4444_Premultiplied;
        break;
    case SCREEN_FORMAT_RGBX5551:
        imageFormat = QImage::Format_RGB555;
        break;
    case SCREEN_FORMAT_RGB565:
        imageFormat = QImage::Format_RGB16;
        break;
    case SCREEN_FORMAT_RGBX8888:
        imageFormat = QImage::Format_RGB32;
        break;
    case SCREEN_FORMAT_RGBA8888:
        imageFormat = QImage::Format_ARGB32_Premultiplied;
        break;
    default:
        qFatal("QBB: unsupported buffer format, format=%d", screenFormat);
    }

    // wrap buffer in an image
    mImage = QImage(dataPtr, size[0], size[1], stride, imageFormat);
}

QBBBuffer::QBBBuffer(const QBBBuffer &other)
    : mBuffer(other.mBuffer),
      mImage(other.mImage)
{
#if defined(QBBBUFFER_DEBUG)
    qDebug() << "QBBBuffer::QBBBuffer - copy";
#endif
}

QBBBuffer::~QBBBuffer()
{
#if defined(QBBBUFFER_DEBUG)
    qDebug() << "QBBBuffer::~QBBBuffer";
#endif
}

void QBBBuffer::invalidateInCache()
{
#if defined(QBBBUFFER_DEBUG)
    qDebug() << "QBBBuffer::invalidateInCache";
#endif

    // verify native buffer exists
    if (mBuffer == NULL) {
        qFatal("QBB: can't invalidate cache for null buffer");
    }

    // evict buffer's data from cache
    errno = 0;
    int result = msync(mImage.bits(), mImage.height() * mImage.bytesPerLine(), MS_INVALIDATE | MS_CACHE_ONLY);
    if (result != 0) {
        qFatal("QBB: failed to invalidate cache, errno=%d", errno);
    }
}

QT_END_NAMESPACE
