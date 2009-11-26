/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#ifndef QTRIANGULATINGSTROKER_P_H
#define QTRIANGULATINGSTROKER_P_H

#include <private/qdatabuffer_p.h>
#include <qvarlengtharray.h>
#include <private/qvectorpath_p.h>
#include <private/qbezier_p.h>
#include <private/qnumeric_p.h>
#include <private/qmath_p.h>

QT_BEGIN_NAMESPACE

class QTriangulatingStroker
{
public:
    void process(const QVectorPath &path, const QPen &pen);

    inline int vertexCount() const { return m_vertices.size(); }
    inline const float *vertices() const { return m_vertices.data(); }

    inline void setInvScale(qreal invScale) { m_inv_scale = invScale; }

private:
    inline void emitLineSegment(float x, float y, float nx, float ny);
    inline void moveTo(const qreal *pts);
    inline void lineTo(const qreal *pts);
    void cubicTo(const qreal *pts);
    inline void join(const qreal *pts);
    inline void normalVector(float x1, float y1, float x2, float y2, float *nx, float *ny);
    inline void endCap(const qreal *pts);
    inline void arcPoints(float cx, float cy, float fromX, float fromY, float toX, float toY, QVarLengthArray<float> &points);
    void endCapOrJoinClosed(const qreal *start, const qreal *cur, bool implicitClose, bool endsAtStart);


    QDataBuffer<float> m_vertices;

    float m_cx, m_cy;           // current points
    float m_nvx, m_nvy;         // normal vector...
    float m_width;
    qreal m_miter_limit;

    int m_roundness;            // Number of line segments in a round join
    qreal m_sin_theta;          // sin(m_roundness / 360);
    qreal m_cos_theta;          // cos(m_roundness / 360);
    qreal m_inv_scale;
    float m_curvyness_mul;
    float m_curvyness_add;

    Qt::PenJoinStyle m_join_style;
    Qt::PenCapStyle m_cap_style;
};

class QDashedStrokeProcessor
{
public:
    QDashedStrokeProcessor();

    void process(const QVectorPath &path, const QPen &pen);

    inline void addElement(QPainterPath::ElementType type, qreal x, qreal y) {
        m_points.add(x);
        m_points.add(y);
        m_types.add(type);
    }

    inline int elementCount() const { return m_types.size(); }
    inline qreal *points() const { return m_points.data(); }
    inline QPainterPath::ElementType *elementTypes() const { return m_types.data(); }

    inline void setInvScale(qreal invScale) { m_inv_scale = invScale; }

private:
    QDataBuffer<qreal> m_points;
    QDataBuffer<QPainterPath::ElementType> m_types;
    QDashStroker m_dash_stroker;
    qreal m_inv_scale;
};





inline void QTriangulatingStroker::normalVector(float x1, float y1, float x2, float y2,
                                                float *nx, float *ny)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    float pw;

    if (dx == 0)
        pw = m_width / qAbs(dy);
    else if (dy == 0)
        pw = m_width / qAbs(dx);
    else
        pw = m_width / sqrt(dx*dx + dy*dy);

    *nx = -dy * pw;
    *ny = dx * pw;
}



inline void QTriangulatingStroker::emitLineSegment(float x, float y, float vx, float vy)
{
    m_vertices.add(x + vx);
    m_vertices.add(y + vy);
    m_vertices.add(x - vx);
    m_vertices.add(y - vy);
}

inline void QTriangulatingStroker::arcPoints(float cx, float cy, float fromX, float fromY, float toX, float toY, QVarLengthArray<float> &points)
{
    float dx1 = fromX - cx;
    float dy1 = fromY - cy;
    float dx2 = toX - cx;
    float dy2 = toY - cy;

    // while more than 180 degrees left:
    while (dx1 * dy2 - dx2 * dy1 < 0) {
        float tmpx = dx1 * m_cos_theta - dy1 * m_sin_theta;
        float tmpy = dx1 * m_sin_theta + dy1 * m_cos_theta;
        dx1 = tmpx;
        dy1 = tmpy;
        points.append(cx + dx1);
        points.append(cy + dy1);
    }

    // while more than 90 degrees left:
    while (dx1 * dx2 + dy1 * dy2 < 0) {
        float tmpx = dx1 * m_cos_theta - dy1 * m_sin_theta;
        float tmpy = dx1 * m_sin_theta + dy1 * m_cos_theta;
        dx1 = tmpx;
        dy1 = tmpy;
        points.append(cx + dx1);
        points.append(cy + dy1);
    }

    // while more than 0 degrees left:
    while (dx1 * dy2 - dx2 * dy1 > 0) {
        float tmpx = dx1 * m_cos_theta - dy1 * m_sin_theta;
        float tmpy = dx1 * m_sin_theta + dy1 * m_cos_theta;
        dx1 = tmpx;
        dy1 = tmpy;
        points.append(cx + dx1);
        points.append(cy + dy1);
    }

    // remove last point which was rotated beyond [toX, toY].
    if (!points.isEmpty())
        points.resize(points.size() - 2);
}

inline void QTriangulatingStroker::endCap(const qreal *)
{
    switch (m_cap_style) {
    case Qt::FlatCap:
        break;
    case Qt::SquareCap:
        emitLineSegment(m_cx + m_nvy, m_cy - m_nvx, m_nvx, m_nvy);
        break;
    case Qt::RoundCap: {
        QVarLengthArray<float> points;
        int count = m_vertices.size();
        arcPoints(m_cx, m_cy, m_vertices.at(count - 2), m_vertices.at(count - 1), m_vertices.at(count - 4), m_vertices.at(count - 3), points);
        int front = 0;
        int end = points.size() / 2;
        while (front != end) {
            m_vertices.add(points[2 * end - 2]);
            m_vertices.add(points[2 * end - 1]);
            --end;
            if (front == end)
                break;
            m_vertices.add(points[2 * front + 0]);
            m_vertices.add(points[2 * front + 1]);
            ++front;
        }
        break; }
    default: break; // to shut gcc up...
    }
}


void QTriangulatingStroker::moveTo(const qreal *pts)
{
    m_cx = pts[0];
    m_cy = pts[1];

    float x2 = pts[2];
    float y2 = pts[3];
    normalVector(m_cx, m_cy, x2, y2, &m_nvx, &m_nvy);


    // To acheive jumps we insert zero-area tringles. This is done by
    // adding two identical points in both the end of previous strip
    // and beginning of next strip
    bool invisibleJump = m_vertices.size();

    switch (m_cap_style) {
    case Qt::FlatCap:
        if (invisibleJump) {
            m_vertices.add(m_cx + m_nvx);
            m_vertices.add(m_cy + m_nvy);
        }
        break;
    case Qt::SquareCap: {
        float sx = m_cx - m_nvy;
        float sy = m_cy + m_nvx;
        if (invisibleJump) {
            m_vertices.add(sx + m_nvx);
            m_vertices.add(sy + m_nvy);
        }
        emitLineSegment(sx, sy, m_nvx, m_nvy);
        break; }
    case Qt::RoundCap: {
        QVarLengthArray<float> points;
        arcPoints(m_cx, m_cy, m_cx + m_nvx, m_cy + m_nvy, m_cx - m_nvx, m_cy - m_nvy, points);
        m_vertices.resize(m_vertices.size() + points.size() + 2 * int(invisibleJump));
        int count = m_vertices.size();
        int front = 0;
        int end = points.size() / 2;
        while (front != end) {
            m_vertices.at(--count) = points[2 * end - 1];
            m_vertices.at(--count) = points[2 * end - 2];
            --end;
            if (front == end)
                break;
            m_vertices.at(--count) = points[2 * front + 1];
            m_vertices.at(--count) = points[2 * front + 0];
            ++front;
        }

        if (invisibleJump) {
            m_vertices.at(count - 1) = m_vertices.at(count + 1);
            m_vertices.at(count - 2) = m_vertices.at(count + 0);
        }
        break; }
    default: break; // ssssh gcc...
    }
    emitLineSegment(m_cx, m_cy, m_nvx, m_nvy);
}



void QTriangulatingStroker::lineTo(const qreal *pts)
{
    emitLineSegment(pts[0], pts[1], m_nvx, m_nvy);
    m_cx = pts[0];
    m_cy = pts[1];
}



void QTriangulatingStroker::join(const qreal *pts)
{
    // Creates a join to the next segment (m_cx, m_cy) -> (pts[0], pts[1])
    normalVector(m_cx, m_cy, pts[0], pts[1], &m_nvx, &m_nvy);

    switch (m_join_style) {
    case Qt::BevelJoin:
        break;
    case Qt::MiterJoin: {
        // Find out on which side the join should be.
        int count = m_vertices.size();
        float prevNvx = m_vertices.at(count - 2) - m_cx;
        float prevNvy = m_vertices.at(count - 1) - m_cy;
        float xprod = prevNvx * m_nvy - prevNvy * m_nvx;
        float px, py, qx, qy;

        // If the segments are parallel, use bevel join.
        if (qFuzzyIsNull(xprod))
            break;

        // Find the corners of the previous and next segment to join.
        if (xprod < 0) {
            px = m_vertices.at(count - 2);
            py = m_vertices.at(count - 1);
            qx = m_cx - m_nvx;
            qy = m_cy - m_nvy;
        } else {
            px = m_vertices.at(count - 4);
            py = m_vertices.at(count - 3);
            qx = m_cx + m_nvx;
            qy = m_cy + m_nvy;
        }

        // Find intersection point.
        float pu = px * prevNvx + py * prevNvy;
        float qv = qx * m_nvx + qy * m_nvy;
        float ix = (m_nvy * pu - prevNvy * qv) / xprod;
        float iy = (prevNvx * qv - m_nvx * pu) / xprod;

        // Check that the distance to the intersection point is less than the miter limit.
        if ((ix - px) * (ix - px) + (iy - py) * (iy - py) <= m_miter_limit * m_miter_limit) {
            m_vertices.add(ix);
            m_vertices.add(iy);
            m_vertices.add(ix);
            m_vertices.add(iy);
        }
        // else
        // Do a plain bevel join if the miter limit is exceeded or if
        // the lines are parallel. This is not what the raster
        // engine's stroker does, but it is both faster and similar to
        // what some other graphics API's do.

        break; }
    case Qt::RoundJoin: {
        QVarLengthArray<float> points;
        int count = m_vertices.size();
        float prevNvx = m_vertices.at(count - 2) - m_cx;
        float prevNvy = m_vertices.at(count - 1) - m_cy;
        if (m_nvx * prevNvy - m_nvy * prevNvx < 0) {
            arcPoints(0, 0, m_nvx, m_nvy, -prevNvx, -prevNvy, points);
            for (int i = points.size() / 2; i > 0; --i)
                emitLineSegment(m_cx, m_cy, points[2 * i - 2], points[2 * i - 1]);
        } else {
            arcPoints(0, 0, -prevNvx, -prevNvy, m_nvx, m_nvy, points);
            for (int i = 0; i < points.size() / 2; ++i)
                emitLineSegment(m_cx, m_cy, points[2 * i + 0], points[2 * i + 1]);
        }
        break; }
    default: break; // gcc warn--
    }

    emitLineSegment(m_cx, m_cy, m_nvx, m_nvy);
}

QT_END_NAMESPACE

#endif
