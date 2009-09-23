/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include <private/qpaintengine_s60_p.h>
#include <private/qpixmap_s60_p.h>
#include <private/qt_s60_p.h>

QT_BEGIN_NAMESPACE

class QS60PaintEnginePrivate : public QRasterPaintEnginePrivate
{
public:
    QS60PaintEnginePrivate(QS60PaintEngine *engine) { }
};

QS60PaintEngine::QS60PaintEngine(QPaintDevice *device, QS60PixmapData *data)
    : QRasterPaintEngine(*(new QS60PaintEnginePrivate(this)), device), pixmapData(data)
{
}

bool QS60PaintEngine::begin(QPaintDevice *device)
{
    pixmapData->beginDataAccess();
    return QRasterPaintEngine::begin(device);
}

bool QS60PaintEngine::end()
{
    bool ret = QRasterPaintEngine::end();
    pixmapData->endDataAccess();
    return ret;
}

void QS60PaintEngine::drawPixmap(const QPointF &p, const QPixmap &pm)
{
    QS60PixmapData *srcData = static_cast<QS60PixmapData *>(pm.pixmapData());
    srcData->beginDataAccess();
    QRasterPaintEngine::drawPixmap(p, pm);
    srcData->endDataAccess();
}

void QS60PaintEngine::drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr)
{
    QS60PixmapData *srcData = static_cast<QS60PixmapData *>(pm.pixmapData());
    srcData->beginDataAccess();
    QRasterPaintEngine::drawPixmap(r, pm, sr);
    srcData->endDataAccess();
}

void QS60PaintEngine::drawTiledPixmap(const QRectF &r, const QPixmap &pm, const QPointF &sr)
{
    QS60PixmapData *srcData = static_cast<QS60PixmapData *>(pm.pixmapData());
    srcData->beginDataAccess();
    QRasterPaintEngine::drawTiledPixmap(r, pm, sr);
    srcData->endDataAccess();
}

void QS60PaintEngine::prepare(QImage *image)
{
    QRasterBuffer *buffer = d_func()->rasterBuffer.data();
    if (buffer)
        buffer->prepare(image);
}

QT_END_NAMESPACE
