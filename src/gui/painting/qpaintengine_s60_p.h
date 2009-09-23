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

#ifndef QPAINTENGINE_S60_P_H
#define QPAINTENGINE_S60_P_H

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

#include "private/qpaintengine_raster_p.h"

QT_BEGIN_NAMESPACE

class QS60PaintEnginePrivate;
class QS60PixmapData;

class QS60PaintEngine : public QRasterPaintEngine
{
    Q_DECLARE_PRIVATE(QS60PaintEngine)

public:
    QS60PaintEngine(QPaintDevice *device, QS60PixmapData* data);
    bool begin(QPaintDevice *device);
    bool end();

    void drawPixmap(const QPointF &p, const QPixmap &pm);
    void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr);
    void drawTiledPixmap(const QRectF &r, const QPixmap &pm, const QPointF &sr);

    void prepare(QImage* image);

private:
    QS60PixmapData *pixmapData;
};

QT_END_NAMESPACE

#endif // QPAINTENGINE_S60_P_H
