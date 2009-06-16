/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPAINTENGINE_DIRECTFB_P_H
#define QPAINTENGINE_DIRECTFB_P_H

#include <QtGui/qpaintengine.h>
#include <private/qpaintengine_raster_p.h>

QT_BEGIN_HEADER

QT_MODULE(Gui)

class QDirectFBPaintEnginePrivate;

class QDirectFBPaintEngine : public QRasterPaintEngine
{
    Q_DECLARE_PRIVATE(QDirectFBPaintEngine)
public:
    QDirectFBPaintEngine(QPaintDevice *device);
    ~QDirectFBPaintEngine();

    bool begin(QPaintDevice *device);
    bool end();

    void drawRects(const QRect  *rects, int rectCount);
    void drawRects(const QRectF *rects, int rectCount);

    void fillRect(const QRectF &r, const QBrush &brush);
    void fillRect(const QRectF &r, const QColor &color);

    void drawLines(const QLine *line, int lineCount);
    void drawLines(const QLineF *line, int lineCount);

    void drawImage(const QPointF &p, const QImage &img);
    void drawImage(const QRectF &r, const QImage &pm, const QRectF &sr,
                   Qt::ImageConversionFlags falgs = Qt::AutoColor);

    void drawPixmap(const QPointF &p, const QPixmap &pm);
    void drawPixmap(const QRectF &r, const QPixmap &pixmap, const QRectF &sr);
    void drawTiledPixmap(const QRectF &r, const QPixmap &pm, const QPointF &sr);

    void drawColorSpans(const QSpan *spans, int count, uint color);
    void drawBufferSpan(const uint *buffer, int bufsize,
                        int x, int y, int length, uint const_alpha);


    // The following methods simply lock the surface & call the base implementation
    void stroke(const QVectorPath &path, const QPen &pen);
    void drawPath(const QPainterPath &path);
    void drawPoints(const QPointF *points, int pointCount);
    void drawPoints(const QPoint *points, int pointCount);
    void drawEllipse(const QRectF &rect);
    void drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode);
    void drawPolygon(const QPoint *points, int pointCount, PolygonDrawMode mode);
    void drawTextItem(const QPointF &p, const QTextItem &textItem);
    void fill(const QVectorPath &path, const QBrush &brush);

    virtual void clipEnabledChanged();
    virtual void penChanged();
    virtual void opacityChanged();
    virtual void compositionModeChanged();
    virtual void renderHintsChanged();
    virtual void transformChanged();

    virtual void setState(QPainterState *state);

    virtual void clip(const QVectorPath &path, Qt::ClipOperation op);
    virtual void clip(const QRect &rect, Qt::ClipOperation op);

    static void initImageCache(int size);
};

QT_END_HEADER

#endif // QPAINTENGINE_DIRECTFB_P_H
