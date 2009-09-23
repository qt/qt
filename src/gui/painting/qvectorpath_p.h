/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QVECTORPATH_P_H
#define QVECTORPATH_P_H

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

#include <QtGui/qpaintengine.h>

#include <private/qpaintengine_p.h>
#include <private/qstroker_p.h>
#include <private/qpainter_p.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)


#define QVECTORPATH_NO_CACHE

struct QRealRect {
    qreal x1, y1, x2, y2;
};

class Q_GUI_EXPORT QVectorPath
{
public:
    enum Hint {
        // Basic shapes...
        LinesHint               = 0x0001, // Just plain lines...
        RectangleHint           = 0x0002,
        ConvexPolygonHint       = 0x0003, // Convex polygon...
        NonISectPolygonHint     = 0x0004, // concave polygon, but not intersecting..
        NonCurvedShapeHint      = 0x0005, // Generic polygon, possibly self-intersecting...
        CurvedShapeHint         = 0x0006, // Generic vector path..
        EllipseHint             = 0x0007,
        ShapeHintMask           = 0x000f,

        // Other hints
        CacheHint               = 0x0100,
        ControlPointRect        = 0x0200, // Set if the control point rect has been calculated...

        // Shape rendering specifiers...
        OddEvenFill             = 0x1000,
        WindingFill             = 0x2000,
        ImplicitClose           = 0x4000
    };

    // ### Falcon: introduca a struct XY for points so lars is not so confused...
    QVectorPath(const qreal *points,
                int count,
                const QPainterPath::ElementType *elements = 0,
                uint hints = CurvedShapeHint)
        : m_elements(elements),
          m_points(points),
          m_count(count),
          m_hints(hints)
#ifndef QVECTORPATH_NO_CACHE
        , m_cache(0)
#endif
    {
    }

    QRectF controlPointRect() const;

    inline Hint shape() const { return (Hint) (m_hints & ShapeHintMask); }

    inline bool hasCacheHint() const { return m_hints & CacheHint; }
    inline bool hasImplicitClose() const { return m_hints & ImplicitClose; }
    inline bool hasWindingFill() const { return m_hints & WindingFill; }

    inline uint hints() const { return m_hints; }

    inline const QPainterPath::ElementType *elements() const { return m_elements; }
    inline const qreal *points() const { return m_points; }
    inline bool isEmpty() const { return m_points == 0; }

    inline int elementCount() const { return m_count; }
    inline const QPainterPath convertToPainterPath() const;

    static inline uint polygonFlags(QPaintEngine::PolygonDrawMode mode);

private:
    Q_DISABLE_COPY(QVectorPath)

#ifndef QVECTORPATH_NO_CACHE
    struct CacheEntry {
        void *engine;
        int id;
        void *extra;
        CacheEntry *next;
    };

    void addCacheData(CacheEntry *d) {
        d->next = m_cache;
        m_cache = d;
    }

    CacheEntry *m_cache;
#endif

    const QPainterPath::ElementType *m_elements;
    const qreal *m_points;
    const int m_count;

    mutable uint m_hints;
    mutable QRealRect m_cp_rect;
};

Q_GUI_EXPORT const QVectorPath &qtVectorPathForPath(const QPainterPath &path);

QT_END_NAMESPACE

QT_END_HEADER

#endif
