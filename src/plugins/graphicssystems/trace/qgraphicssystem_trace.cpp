/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights.  These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
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

#include "qgraphicssystem_trace_p.h"
#include <private/qpixmap_raster_p.h>
#include <private/qpaintbuffer_p.h>
#include <private/qwindowsurface_raster_p.h>

#include <QFile>
#include <QPainter>
#include <QtDebug>

QT_BEGIN_NAMESPACE

class QTraceWindowSurface : public QRasterWindowSurface
{
public:
    QTraceWindowSurface(QWidget *widget);
    ~QTraceWindowSurface();

    QPaintDevice *paintDevice();
    void endPaint(const QRegion &rgn);

private:
    QPaintBuffer *buffer;

    QFile *outputFile;
    QDataStream *out;

    int frameId;
};

QTraceWindowSurface::QTraceWindowSurface(QWidget *widget)
    : QRasterWindowSurface(widget)
    , buffer(0)
    , outputFile(0)
    , out(0)
    , frameId(0)
{
}

QTraceWindowSurface::~QTraceWindowSurface()
{
    delete out;
    delete outputFile;
}

QPaintDevice *QTraceWindowSurface::paintDevice()
{
    if (!buffer) {
        buffer = new QPaintBuffer;
        buffer->setBoundingRect(geometry());
    }
    return buffer;
}

void QTraceWindowSurface::endPaint(const QRegion &rgn)
{
    if (!out) {
        outputFile = new QFile(QString(QLatin1String("qtgraphics-%0.trace")).arg((qulonglong)window()->winId()));
        if (outputFile->open(QIODevice::WriteOnly))
            out = new QDataStream(outputFile);
    }

    QPainter p(QRasterWindowSurface::paintDevice());
    buffer->draw(&p);
    p.end();

    if (out) {
        *out << frameId++;
        *out << (qulonglong)window()->winId();
        *out << geometry();
        *out << rgn;
        *out << *buffer;
    }

    delete buffer;
    buffer = 0;
}

QTraceGraphicsSystem::QTraceGraphicsSystem()
{
}

QPixmapData *QTraceGraphicsSystem::createPixmapData(QPixmapData::PixelType type) const
{
    return new QRasterPixmapData(type);
}

QWindowSurface *QTraceGraphicsSystem::createWindowSurface(QWidget *widget) const
{
    return new QTraceWindowSurface(widget);
}

QT_END_NAMESPACE
