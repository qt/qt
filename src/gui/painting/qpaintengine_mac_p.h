/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QPAINTENGINE_MAC_P_H
#define QPAINTENGINE_MAC_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtGui/qpaintengine.h"
#include "private/qt_mac_p.h"
#include "private/qpaintengine_p.h"
#include "private/qpolygonclipper_p.h"
#include "QtCore/qhash.h"
#ifndef QT_MAC_NO_QUICKDRAW
#include <private/qwidget_p.h>
#endif

typedef struct CGColorSpace *CGColorSpaceRef;
QT_BEGIN_NAMESPACE

extern int qt_defaultDpi();
extern int qt_defaultDpiX();
extern int qt_defaultDpiY();

#ifndef QT_MAC_NO_QUICKDRAW
class QMacSavedPortInfo
{
    RgnHandle clip;
    GWorldPtr world;
    GDHandle handle;
    PenState pen; //go pennstate
    RGBColor back, fore;
    bool valid_gworld;
    void init();

public:
    inline QMacSavedPortInfo() { init(); }
    inline QMacSavedPortInfo(QPaintDevice *pd) { init(); setPaintDevice(pd); }
    inline QMacSavedPortInfo(QPaintDevice *pd, const QRect &r)
        { init(); setPaintDevice(pd); setClipRegion(r); }
    inline QMacSavedPortInfo(QPaintDevice *pd, const QRegion &r)
        { init(); setPaintDevice(pd); setClipRegion(r); }
    ~QMacSavedPortInfo();
    static inline bool setClipRegion(const QRect &r);
    static inline bool setClipRegion(const QRegion &r);
    static inline bool setPaintDevice(QPaintDevice *);
};

inline bool
QMacSavedPortInfo::setClipRegion(const QRect &rect)
{
    Rect r;
    SetRect(&r, rect.x(), rect.y(), rect.right()+1, rect.bottom()+1);
    ClipRect(&r);
    return true;
}

inline bool
QMacSavedPortInfo::setClipRegion(const QRegion &r)
{
    if(r.isEmpty())
        return setClipRegion(QRect());
    QMacSmartQuickDrawRegion rgn(r.toQDRgn());
    SetClip(rgn);
    return true;
}

inline bool
QMacSavedPortInfo::setPaintDevice(QPaintDevice *pd)
{
    if(!pd)
        return false;
    bool ret = true;
    extern GrafPtr qt_mac_qd_context(const QPaintDevice *); // qpaintdevice_mac.cpp
    if(pd->devType() == QInternal::Widget)
        SetPortWindowPort(qt_mac_window_for(static_cast<QWidget*>(pd)));
    else if(pd->devType() == QInternal::Pixmap || pd->devType() == QInternal::Printer)
        SetGWorld((GrafPtr)qt_mac_qd_context(pd), 0); //set the gworld
    return ret;
}

inline void
QMacSavedPortInfo::init()
{
    GetBackColor(&back);
    GetForeColor(&fore);
    GetGWorld(&world, &handle);
    valid_gworld = true;
    clip = NewRgn();
    GetClip(clip);
    GetPenState(&pen);
}

inline QMacSavedPortInfo::~QMacSavedPortInfo()
{
    bool set_state = false;
    if(valid_gworld) {
        set_state = IsValidPort(world);
        if(set_state)
            SetGWorld(world,handle); //always do this one first
    } else {
        setPaintDevice(qt_mac_safe_pdev);
    }
    if(set_state) {
        SetClip(clip);
        SetPenState(&pen);
        RGBForeColor(&fore);
        RGBBackColor(&back);
    }
    DisposeRgn(clip);
}
#else
class QMacSavedPortInfo
{
public:
    inline QMacSavedPortInfo() { }
    inline QMacSavedPortInfo(QPaintDevice *) { }
    inline QMacSavedPortInfo(QPaintDevice *, const QRect &) { }
    inline QMacSavedPortInfo(QPaintDevice *, const QRegion &) { }
    ~QMacSavedPortInfo() { }
    static inline bool setClipRegion(const QRect &) { return false; }
    static inline bool setClipRegion(const QRegion &) { return false; }
    static inline bool setPaintDevice(QPaintDevice *) { return false; }
};
#endif

class QCoreGraphicsPaintEnginePrivate;
class QCoreGraphicsPaintEngine : public QPaintEngine
{
    Q_DECLARE_PRIVATE(QCoreGraphicsPaintEngine)

public:
    QCoreGraphicsPaintEngine();
    ~QCoreGraphicsPaintEngine();

    bool begin(QPaintDevice *pdev);
    bool end();
    static CGColorSpaceRef macGenericColorSpace();
    static CGColorSpaceRef macDisplayColorSpace(const QWidget *widget = 0);

    void updateState(const QPaintEngineState &state);

    void updatePen(const QPen &pen);
    void updateBrush(const QBrush &brush, const QPointF &pt);
    void updateFont(const QFont &font);
    void updateOpacity(qreal opacity);
    void updateMatrix(const QTransform &matrix);
    void updateTransform(const QTransform &matrix);
    void updateClipRegion(const QRegion &region, Qt::ClipOperation op);
    void updateClipPath(const QPainterPath &path, Qt::ClipOperation op);
    void updateCompositionMode(QPainter::CompositionMode mode);
    void updateRenderHints(QPainter::RenderHints hints);

    void drawLines(const QLineF *lines, int lineCount);
    void drawRects(const QRectF *rects, int rectCount);
    void drawPoints(const QPointF *p, int pointCount);
    void drawEllipse(const QRectF &r);
    void drawPath(const QPainterPath &path);

    void drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode);
    void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr);
    void drawTiledPixmap(const QRectF &r, const QPixmap &pixmap, const QPointF &s);

    void drawTextItem(const QPointF &pos, const QTextItem &item);
    void drawImage(const QRectF &r, const QImage &pm, const QRectF &sr,
                   Qt::ImageConversionFlags flags = Qt::AutoColor);

    Type type() const { return QPaintEngine::CoreGraphics; }

    CGContextRef handle() const;

    static void initialize();
    static void cleanup();

    QPainter::RenderHints supportedRenderHints() const;

    //avoid partial shadowed overload warnings...
    void drawLines(const QLine *lines, int lineCount) { QPaintEngine::drawLines(lines, lineCount); }
    void drawRects(const QRect *rects, int rectCount) { QPaintEngine::drawRects(rects, rectCount); }
    void drawPoints(const QPoint *p, int pointCount) { QPaintEngine::drawPoints(p, pointCount); }
    void drawEllipse(const QRect &r) { QPaintEngine::drawEllipse(r); }
    void drawPolygon(const QPoint *points, int pointCount, PolygonDrawMode mode)
    { QPaintEngine::drawPolygon(points, pointCount, mode); }

protected:
    friend class QMacPrintEngine;
    friend class QMacPrintEnginePrivate;
    friend void qt_mac_display_change_callbk(CGDirectDisplayID, CGDisplayChangeSummaryFlags, void *);
    QCoreGraphicsPaintEngine(QPaintEnginePrivate &dptr);

private:
    static bool m_postRoutineRegistered;
    static CGColorSpaceRef m_genericColorSpace;
    static QHash<CGDirectDisplayID, CGColorSpaceRef> m_displayColorSpaceHash;
    static void cleanUpMacColorSpaces();
    Q_DISABLE_COPY(QCoreGraphicsPaintEngine)
};

/*****************************************************************************
  Private data
 *****************************************************************************/
class QCoreGraphicsPaintEnginePrivate : public QPaintEnginePrivate
{
    Q_DECLARE_PUBLIC(QCoreGraphicsPaintEngine)
public:
    QCoreGraphicsPaintEnginePrivate()
        : hd(0), shading(0), stackCount(0), complexXForm(false)
    {
    }

    struct {
        QPen pen;
        QBrush brush;
        uint clipEnabled : 1;
        QRegion clip;
        QTransform transform;
   } current;

    //state info (shared with QD)
    CGAffineTransform orig_xform;

    //cg structures
    CGContextRef hd;
    CGShadingRef shading;
    int stackCount;
    bool complexXForm;
    enum { CosmeticNone, CosmeticTransformPath, CosmeticSetPenWidth } cosmeticPen;

    // pixel and cosmetic pen size in user coordinates.
    QPointF pixelSize;
    float cosmeticPenSize;

    //internal functions
    enum { CGStroke=0x01, CGEOFill=0x02, CGFill=0x04 };
    void drawPath(uchar ops, CGMutablePathRef path = 0);
    void setClip(const QRegion *rgn=0);
    void resetClip();
    void setFillBrush(const QPointF &origin=QPoint());
    void setStrokePen(const QPen &pen);
    inline void saveGraphicsState();
    inline void restoreGraphicsState();
    float penOffset();
    QPointF devicePixelSize(CGContextRef context);
    float adjustPenWidth(float penWidth);
    inline void setTransform(const QTransform *matrix=0)
    {
        CGContextConcatCTM(hd, CGAffineTransformInvert(CGContextGetCTM(hd)));
        CGAffineTransform xform = orig_xform;
        if(matrix) {
            extern CGAffineTransform qt_mac_convert_transform_to_cg(const QTransform &);
            xform = CGAffineTransformConcat(qt_mac_convert_transform_to_cg(*matrix), xform);
        }
        CGContextConcatCTM(hd, xform);
        CGContextSetTextMatrix(hd, xform);
    }
};

inline void QCoreGraphicsPaintEnginePrivate::saveGraphicsState()
{
    ++stackCount;
    CGContextSaveGState(hd);
}

inline void QCoreGraphicsPaintEnginePrivate::restoreGraphicsState()
{
    --stackCount;
    Q_ASSERT(stackCount >= 0);
    CGContextRestoreGState(hd);
}

class QMacQuartzPaintDevice : public QPaintDevice
{
public:
    QMacQuartzPaintDevice(CGContextRef cg, int width, int height, int bytesPerLine)
        : mCG(cg), mWidth(width), mHeight(height), mBytesPerLine(bytesPerLine)
    { }
    int devType() const { return QInternal::MacQuartz; }
    CGContextRef cgContext() const { return mCG; }
    int metric(PaintDeviceMetric metric) const {
        switch (metric) {
        case PdmWidth:
            return mWidth;
        case PdmHeight:
            return mHeight;
        case PdmWidthMM:
            return (qt_defaultDpiX() * mWidth) / 2.54;
        case PdmHeightMM:
            return (qt_defaultDpiY() * mHeight) / 2.54;
        case PdmNumColors:
            return 0;
        case PdmDepth:
            return 32;
        case PdmDpiX:
        case PdmPhysicalDpiX:
            return qt_defaultDpiX();
        case PdmDpiY:
        case PdmPhysicalDpiY:
            return qt_defaultDpiY();
        }
        return 0;
    }
    QPaintEngine *paintEngine() const { qWarning("This function should never be called."); return 0; }
private:
    CGContextRef mCG;
    int mWidth;
    int mHeight;
    int mBytesPerLine;
};

QT_END_NAMESPACE

#endif // QPAINTENGINE_MAC_P_H
