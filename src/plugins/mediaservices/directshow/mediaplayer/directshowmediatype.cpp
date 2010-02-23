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

#include "directshowmediatype.h"


QT_BEGIN_NAMESPACE

namespace
{
    struct TypeLookup
    {
        QVideoFrame::PixelFormat pixelFormat;
        GUID mediaType;
    };

    static const TypeLookup qt_typeLookup[] =
    {
        { QVideoFrame::Format_RGB32, MEDIASUBTYPE_RGB32 },
        { QVideoFrame::Format_BGR24, MEDIASUBTYPE_RGB24 },
        { QVideoFrame::Format_RGB565, MEDIASUBTYPE_RGB565 },
        { QVideoFrame::Format_RGB555, MEDIASUBTYPE_RGB555 },
        { QVideoFrame::Format_AYUV444, MEDIASUBTYPE_AYUV },
        { QVideoFrame::Format_YUYV, MEDIASUBTYPE_YUY2 },
        { QVideoFrame::Format_UYVY, MEDIASUBTYPE_UYVY },
        { QVideoFrame::Format_IMC1, MEDIASUBTYPE_IMC1 },
        { QVideoFrame::Format_IMC2, MEDIASUBTYPE_IMC2 },
        { QVideoFrame::Format_IMC3, MEDIASUBTYPE_IMC3 },
        { QVideoFrame::Format_IMC4, MEDIASUBTYPE_IMC4 },
        { QVideoFrame::Format_YV12, MEDIASUBTYPE_YV12 },
        { QVideoFrame::Format_NV12, MEDIASUBTYPE_NV12 },
        { QVideoFrame::Format_YUV420P, MEDIASUBTYPE_IYUV }
    };
}

void DirectShowMediaType::copy(AM_MEDIA_TYPE *target, const AM_MEDIA_TYPE &source)
{
    *target = source;

    if (source.cbFormat > 0) {
        target->pbFormat = reinterpret_cast<PBYTE>(CoTaskMemAlloc(source.cbFormat));
        memcpy(target->pbFormat, source.pbFormat, source.cbFormat);
    }
    if (target->pUnk)
        target->pUnk->AddRef();
}

void DirectShowMediaType::deleteType(AM_MEDIA_TYPE *type)
{
    freeData(type);

    CoTaskMemFree(type);
}

void DirectShowMediaType::freeData(AM_MEDIA_TYPE *type)
{
    if (type->cbFormat > 0)
        CoTaskMemFree(type->pbFormat);

    if (type->pUnk)
        type->pUnk->Release();
}


GUID DirectShowMediaType::convertPixelFormat(QVideoFrame::PixelFormat format)
{
    const int count = sizeof(qt_typeLookup) / sizeof(TypeLookup);

    for (int i = 0; i < count; ++i)
        if (qt_typeLookup[i].pixelFormat == format)
            return qt_typeLookup[i].mediaType;
    return MEDIASUBTYPE_None;
}

QVideoSurfaceFormat DirectShowMediaType::formatFromType(const AM_MEDIA_TYPE &type)
{
    const int count = sizeof(qt_typeLookup) / sizeof(TypeLookup);

    for (int i = 0; i < count; ++i) {
        if (IsEqualGUID(qt_typeLookup[i].mediaType, type.subtype) && type.cbFormat > 0) {
            if (IsEqualGUID(type.formattype, FORMAT_VideoInfo)) {
                VIDEOINFOHEADER *header = reinterpret_cast<VIDEOINFOHEADER *>(type.pbFormat);

                QVideoSurfaceFormat format(
                        QSize(header->bmiHeader.biWidth, qAbs(header->bmiHeader.biHeight)),
                        qt_typeLookup[i].pixelFormat);

                if (header->AvgTimePerFrame > 0)
                    format.setFrameRate(10000 /header->AvgTimePerFrame);

                format.setScanLineDirection(header->bmiHeader.biHeight < 0
                        ? QVideoSurfaceFormat::TopToBottom
                        : QVideoSurfaceFormat::BottomToTop);

                return format;
            } else if (IsEqualGUID(type.formattype, FORMAT_VideoInfo2)) {
                VIDEOINFOHEADER2 *header = reinterpret_cast<VIDEOINFOHEADER2 *>(type.pbFormat);

                QVideoSurfaceFormat format(
                        QSize(header->bmiHeader.biWidth, qAbs(header->bmiHeader.biHeight)),
                        qt_typeLookup[i].pixelFormat);

                if (header->AvgTimePerFrame > 0)
                    format.setFrameRate(10000 / header->AvgTimePerFrame);

                format.setScanLineDirection(header->bmiHeader.biHeight < 0
                        ? QVideoSurfaceFormat::TopToBottom
                        : QVideoSurfaceFormat::BottomToTop);

                return format;
            }
        }
    }
    return QVideoSurfaceFormat();
}

int DirectShowMediaType::bytesPerLine(const QVideoSurfaceFormat &format)
{
    switch (format.pixelFormat()) {
    // 32 bpp packed formats.
    case QVideoFrame::Format_RGB32:
    case QVideoFrame::Format_AYUV444:
        return format.frameWidth() * 4;
    // 24 bpp packed formats.
    case QVideoFrame::Format_RGB24:
        return format.frameWidth() * 3 + 3 - format.frameWidth() % 4;
    // 16 bpp packed formats.
    case QVideoFrame::Format_RGB565:
    case QVideoFrame::Format_RGB555:
    case QVideoFrame::Format_YUYV:
    case QVideoFrame::Format_UYVY:
        return format.frameWidth() * 2 + 3 - format.frameWidth() % 4;
    // Planar formats.
    case QVideoFrame::Format_IMC1:
    case QVideoFrame::Format_IMC2:
    case QVideoFrame::Format_IMC3:
    case QVideoFrame::Format_IMC4:
    case QVideoFrame::Format_YV12:
    case QVideoFrame::Format_NV12:
    case QVideoFrame::Format_YUV420P:
        return format.frameWidth() + 3 - format.frameWidth() % 4;
    default:
        return 0;
    }
}

QT_END_NAMESPACE
