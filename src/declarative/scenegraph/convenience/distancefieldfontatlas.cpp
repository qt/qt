/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "distancefieldfontatlas_p.h"

#include <qapplication.h>
#include <qfileinfo.h>
#include <qmath.h>
#include <qlibraryinfo.h>

void qt_disableFontHinting(QFont &font)
{
    QFontEngine *fontEngine = QFontPrivate::get(font)->engineForScript(QUnicodeTables::Common);
    if (fontEngine->type() == QFontEngine::Multi) {
        QFontEngineMulti *fem = static_cast<QFontEngineMulti *>(fontEngine);
        fontEngine = fem->engine(0);
    }
    fontEngine->setDefaultHintStyle(QFontEngine::HintNone);
}

#define QT_DISTANCEFIELD_TEXTURESIZE 2048
#define QT_DISTANCEFIELD_BASEFONTSIZE 54
#define QT_DISTANCEFIELD_TILESIZE 64
#define QT_DISTANCEFIELD_SCALE 16
#define QT_DISTANCEFIELD_RADIUS 80
#define QT_DISTANCEFIELD_MARGIN 50
#define QT_DISTANCEFIELD_MARGIN_THRESHOLD 0.31

struct Point
{
    float x, y;
};

struct Vertex
{
    Point p;
    float d;
};

void fillTrapezoid(QRgb *bits, int width, int height, int fromY, int toY,
                   const Vertex *left1, const Vertex *left2,
                   const Vertex *right1, const Vertex *right2)
{
    fromY = qMax(0, fromY);
    toY = qMin(height, toY);

    if (toY <= fromY)
        return;

    float leftDx = (left2->p.x - left1->p.x) / (left2->p.y - left1->p.y);
    float leftDd = (left2->d - left1->d) / (left2->p.y - left1->p.y);
    float leftX = left1->p.x + (fromY - left1->p.y) * leftDx;
    float leftD = left1->d + (fromY - left1->p.y) * leftDd;

    float rightDx = (right2->p.x - right1->p.x) / (right2->p.y - right1->p.y);
    float rightDd = (right2->d - right1->d) / (right2->p.y - right1->p.y);
    float rightX = right1->p.x + (fromY - right1->p.y) * rightDx;
    float rightD = right1->d + (fromY - right1->p.y) * rightDd;

    bits += width * fromY;
    for (int y = fromY; y < toY; ++y, leftX += leftDx, leftD += leftDd, rightX += rightDx, rightD += rightDd, bits += width) {
        int fromX = qMax(0, qCeil(leftX));
        int toX = qMin(width, qCeil(rightX));
        if (toX <= fromX)
            continue;
        float dd = (leftD - rightD) / (leftX - rightX);
        float d = leftD + (fromX - leftX) * dd;
        for (int x = fromX; x < toX; ++x, d += dd) {
            if (abs(d) < abs((bits[x] >> 24) - 127))
                bits[x] = QRgb(d + 127) << 24;
        }
    }
}

bool lineIntersection(Point &result, const Point &left1, const Point &left2, const Point &right1, const Point &right2)
{
    Point u = { left2.x - left1.x, left2.y - left1.y };
    Point v = { right2.x - right1.x, right2.y - right1.y };
    float uxv = u.x * v.y - u.y * v.x;
    if (uxv == 0)
        return false;
    Point d = { left1.x - right1.x, left1.y - right1.y };
    float uxd = u.x * d.y - u.y * d.x;
    float vxd = v.x * d.y - v.y * d.x;
    float t = uxd / uxv;
    float s = vxd / uxv;
    result.x = right1.x + t * v.x;
    result.y = right1.y + t * v.y;
    return t > 0 && t < 1 && s > 0 && s < 1;
}

void drawQuad(QRgb *bits, int width, int height, const Vertex *v1, const Vertex *v2, const Vertex *v3, const Vertex *v4)
{
    float minY = qMin(qMin(v1->p.y, v2->p.y), qMin(v3->p.y, v4->p.y));
    while (v1->p.y > minY) {
        const Vertex *tmp = v1;
        v1 = v2;
        v2 = v3;
        v3 = v4;
        v4 = tmp;
    }
    //   v1
    //  /  \
    // v4  v2
    //  \  /
    //   v3

    if (v2->p.y > v3->p.y && v4->p.y > v3->p.y) {
        // Concave or complex.
        Point p14x32;
        Point p12x34;
        bool int14x32 = lineIntersection(p14x32, v1->p, v4->p, v3->p, v2->p);
        bool int12x34 = lineIntersection(p12x34, v1->p, v2->p, v3->p, v4->p);
        if (int14x32) {
            fillTrapezoid(bits, width, height, qCeil(v1->p.y), qCeil(p14x32.y), v1, v4, v1, v2);
            fillTrapezoid(bits, width, height, qCeil(v3->p.y), qCeil(p14x32.y), v3, v2, v3, v4);
            fillTrapezoid(bits, width, height, qCeil(p14x32.y), qCeil(v2->p.y), v3, v2, v1, v2);
            fillTrapezoid(bits, width, height, qCeil(p14x32.y), qCeil(v4->p.y), v1, v4, v3, v4);
        } else if (int12x34) {
            fillTrapezoid(bits, width, height, qCeil(v1->p.y), qCeil(p12x34.y), v1, v4, v1, v2);
            fillTrapezoid(bits, width, height, qCeil(v3->p.y), qCeil(p12x34.y), v3, v2, v3, v4);
            fillTrapezoid(bits, width, height, qCeil(p12x34.y), qCeil(v4->p.y), v1, v4, v3, v4);
            fillTrapezoid(bits, width, height, qCeil(p12x34.y), qCeil(v2->p.y), v3, v2, v1, v2);
        } else if (v2->p.y > v4->p.y) {
            fillTrapezoid(bits, width, height, qCeil(v1->p.y), qCeil(v4->p.y), v1, v4, v1, v2);
            fillTrapezoid(bits, width, height, qCeil(v3->p.y), qCeil(v4->p.y), v3, v2, v3, v4);
            fillTrapezoid(bits, width, height, qCeil(v4->p.y), qCeil(v2->p.y), v3, v2, v1, v2);
        } else {
            fillTrapezoid(bits, width, height, qCeil(v1->p.y), qCeil(v2->p.y), v1, v4, v1, v2);
            fillTrapezoid(bits, width, height, qCeil(v3->p.y), qCeil(v2->p.y), v3, v2, v3, v4);
            fillTrapezoid(bits, width, height, qCeil(v2->p.y), qCeil(v4->p.y), v1, v4, v3, v4);
        }
    } else if (v2->p.y > v3->p.y) {
        // Right side is straight.
        fillTrapezoid(bits, width, height, qCeil(v1->p.y), qCeil(v4->p.y), v1, v4, v1, v2);
        fillTrapezoid(bits, width, height, qCeil(v4->p.y), qCeil(v3->p.y), v4, v3, v1, v2);
        fillTrapezoid(bits, width, height, qCeil(v3->p.y), qCeil(v2->p.y), v3, v2, v1, v2);
    } else if (v4->p.y > v3->p.y) {
        // Left side is straight.
        fillTrapezoid(bits, width, height, qCeil(v1->p.y), qCeil(v2->p.y), v1, v4, v1, v2);
        fillTrapezoid(bits, width, height, qCeil(v2->p.y), qCeil(v3->p.y), v1, v4, v2, v3);
        fillTrapezoid(bits, width, height, qCeil(v3->p.y), qCeil(v4->p.y), v1, v4, v3, v4);
    } else {
        // Diamond shaped.
        if (v2->p.y > v4->p.y) {
            fillTrapezoid(bits, width, height, qCeil(v1->p.y), qCeil(v4->p.y), v1, v4, v1, v2);
            fillTrapezoid(bits, width, height, qCeil(v4->p.y), qCeil(v2->p.y), v4, v3, v1, v2);
            fillTrapezoid(bits, width, height, qCeil(v2->p.y), qCeil(v3->p.y), v4, v3, v2, v3);
        } else {
            fillTrapezoid(bits, width, height, qCeil(v1->p.y), qCeil(v2->p.y), v1, v4, v1, v2);
            fillTrapezoid(bits, width, height, qCeil(v2->p.y), qCeil(v4->p.y), v1, v4, v2, v3);
            fillTrapezoid(bits, width, height, qCeil(v4->p.y), qCeil(v3->p.y), v4, v3, v2, v3);
        }
    }
}

void drawTriangle(QRgb *bits, int width, int height, const Vertex *v1, const Vertex *v2, const Vertex *v3)
{
    float minY = qMin(qMin(v1->p.y, v2->p.y), v3->p.y);
    while (v1->p.y > minY) {
        const Vertex *tmp = v1;
        v1 = v2;
        v2 = v3;
        v3 = tmp;
    }
    //   v1
    //  /  \
    // v3--v2

    if (v2->p.y > v3->p.y) {
        fillTrapezoid(bits, width, height, qCeil(v1->p.y), qCeil(v3->p.y), v1, v3, v1, v2);
        fillTrapezoid(bits, width, height, qCeil(v3->p.y), qCeil(v2->p.y), v3, v2, v1, v2);
    } else {
        fillTrapezoid(bits, width, height, qCeil(v1->p.y), qCeil(v2->p.y), v1, v3, v1, v2);
        fillTrapezoid(bits, width, height, qCeil(v2->p.y), qCeil(v3->p.y), v1, v3, v2, v3);
    }
}

QImage makeDistanceField(const QPainterPath &path, float offs)
{
    QList<QPolygonF> polys = path.toSubpathPolygons();

    QImage image(QT_DISTANCEFIELD_TILESIZE, QT_DISTANCEFIELD_TILESIZE, QImage::Format_ARGB32_Premultiplied);
    image.fill(0);

    if (polys.isEmpty())
        return image;

    QPainter p(&image);
    p.translate(offs, offs);
    p.scale(1 / qreal(QT_DISTANCEFIELD_SCALE), 1 / qreal(QT_DISTANCEFIELD_SCALE));
    p.fillPath(path, Qt::black);
    p.end();
    QRgb *bits = (QRgb *)image.bits();
    const float angleStep = 15 * 3.141592653589793238f / 180;
    Point rotation = { cos(angleStep), sin(angleStep) };

    int outerPoly = 0;
    int topVertex = 0;
    QVarLengthArray<QVector<Point>, 4> allNormals(polys.count());
    QVarLengthArray<QVector<Vertex>, 4> allVertices(polys.count());
    QVarLengthArray<QVector<bool>, 4> allIsConvex(polys.count());

    for (int i = 0; i < polys.count(); ++i) {
        const QPolygonF &poly = polys.at(i);
        QVector<Point> &normals = allNormals[i];
        QVector<Vertex> &vertices = allVertices[i];
        normals.reserve(poly.count());
        vertices.reserve(3 * poly.count());
        // Calculate vertex "normals".
        for (int next = 0, prev = poly.count() - 1; next < poly.count(); prev = next++) {
            const QPointF &from = poly.at(prev);
            const QPointF &to = poly.at(next);
            Point pos = { float(to.x() / QT_DISTANCEFIELD_SCALE) + offs - 0.5f, float(to.y() / QT_DISTANCEFIELD_SCALE) + offs - 0.5f }; // 0.5f because of rounding used in drawQuad.
            Point normal = { float(to.y() - from.y()), float(from.x() - to.x()) };
            if (normal.x == 0 && normal.y == 0)
                continue;
            float scale = offs / sqrt(normal.x * normal.x + normal.y * normal.y);
            normal.x *= scale;
            normal.y *= scale;
            normals.append(normal);
            Vertex v = { pos, 0 };
            vertices.append(v);
            vertices.append(v);
            vertices.append(v);
        }

        QVector<bool> &isConvex = allIsConvex[i];
        isConvex.resize(normals.count());
        // Normalize normals and calculate vertices.
        for (int next = 0, prev = normals.count() - 1; next < normals.count(); prev = next++) {
            Point avgNormal = {
                normals.at(next).x + normals.at(prev).x,
                normals.at(next).y + normals.at(prev).y
            };
            const Point &normal = normals.at(prev);
            Vertex &intV = vertices[prev * 3];
            Vertex &extV = vertices[prev * 3 + 2];
            if (avgNormal.x != 0 || avgNormal.y != 0) {
                float scale = (offs * offs) / (avgNormal.x * normal.x + avgNormal.y * normal.y);
                avgNormal.x *= scale;
                avgNormal.y *= scale;
                intV.d = offs * QT_DISTANCEFIELD_SCALE;
                extV.d = -offs * QT_DISTANCEFIELD_SCALE;
                intV.p.x -= avgNormal.x;
                intV.p.y -= avgNormal.y;
                extV.p.x += avgNormal.x;
                extV.p.y += avgNormal.y;
            }
            if (vertices.at(prev * 3 + 1).p.y < allVertices.at(outerPoly).at(topVertex * 3 + 1).p.y) {
                outerPoly = i;
                topVertex = prev;
            }
            isConvex[prev] = (normals.at(prev).x * normals.at(next).y - normals.at(prev).y * normals.at(next).x > 0);
        }
        Q_ASSERT(isConvex.count() == normals.count());
        Q_ASSERT(vertices.count() == 3 * normals.count());
    }

    if (!allIsConvex.at(outerPoly).at(topVertex)) {
        for (int i = 0; i < polys.count(); ++i) {
            for (int j = 0; j < allVertices.at(i).count(); ++j)
                allVertices[i][j].d = -allVertices.at(i).at(j).d;
        }
    }

    for (int i = 0; i < polys.count(); ++i) {
        const QVector<Point> &normals = allNormals[i];
        const QVector<Vertex> &vertices = allVertices[i];
        const QVector<bool> &isConvex = allIsConvex[i];
        // Draw quads.
        for (int next = 0, prev = normals.count() - 1; next < normals.count(); prev = next++) {
            Vertex intPrev = vertices.at(prev * 3);
            Vertex extPrev = vertices.at(prev * 3 + 2);
            Vertex intNext = vertices.at(next * 3);
            Vertex extNext = vertices.at(next * 3 + 2);
            if (isConvex.at(prev)) {
                extPrev.p.x = vertices.at(prev * 3 + 1).p.x + normals.at(next).x;
                extPrev.p.y = vertices.at(prev * 3 + 1).p.y + normals.at(next).y;
            } else {
                intPrev.p.x = vertices.at(prev * 3 + 1).p.x - normals.at(next).x;
                intPrev.p.y = vertices.at(prev * 3 + 1).p.y - normals.at(next).y;
            }
            if (isConvex.at(next)) {
                extNext.p.x = vertices.at(next * 3 + 1).p.x + normals.at(next).x;
                extNext.p.y = vertices.at(next * 3 + 1).p.y + normals.at(next).y;
            } else {
                intNext.p.x = vertices.at(next * 3 + 1).p.x - normals.at(next).x;
                intNext.p.y = vertices.at(next * 3 + 1).p.y - normals.at(next).y;
            }

            drawQuad(bits, image.width(), image.height(),
                     &vertices.at(prev * 3 + 1), &extPrev,
                     &extNext, &vertices.at(next * 3 + 1));

            drawQuad(bits, image.width(), image.height(),
                     &intPrev, &vertices.at(prev * 3 + 1),
                     &vertices.at(next * 3 + 1), &intNext);

            if (isConvex.at(prev)) {
                Point n = normals.at(next);
                Vertex v = extPrev;
                for (;;) {
                    Point rn = { n.x * rotation.x + n.y * rotation.y,
                                 n.y * rotation.x - n.x * rotation.y };
                    n = rn;
                    if (n.x * normals.at(prev).y - n.y * normals.at(prev).x >= -0.001) {
                        v.p.x = vertices.at(prev * 3 + 1).p.x + normals.at(prev).x;
                        v.p.y = vertices.at(prev * 3 + 1).p.y + normals.at(prev).y;
                        drawTriangle(bits, image.width(), image.height(), &vertices.at(prev * 3 + 1), &v, &extPrev);
                        break;
                    }

                    v.p.x = vertices.at(prev * 3 + 1).p.x + n.x;
                    v.p.y = vertices.at(prev * 3 + 1).p.y + n.y;
                    drawTriangle(bits, image.width(), image.height(), &vertices.at(prev * 3 + 1), &v, &extPrev);
                    extPrev = v;
                }
            } else {
                Point n = normals.at(next);
                Vertex v = intPrev;
                for (;;) {
                    Point rn = { n.x * rotation.x - n.y * rotation.y,
                                 n.y * rotation.x + n.x * rotation.y };
                    n = rn;
                    if (n.x * normals.at(prev).y - n.y * normals.at(prev).x <= 0.001) {
                        v.p.x = vertices.at(prev * 3 + 1).p.x - normals.at(prev).x;
                        v.p.y = vertices.at(prev * 3 + 1).p.y - normals.at(prev).y;
                        drawTriangle(bits, image.width(), image.height(), &vertices.at(prev * 3 + 1), &intPrev, &v);
                        break;
                    }

                    v.p.x = vertices.at(prev * 3 + 1).p.x - n.x;
                    v.p.y = vertices.at(prev * 3 + 1).p.y - n.y;
                    drawTriangle(bits, image.width(), image.height(), &vertices.at(prev * 3 + 1), &intPrev, &v);
                    intPrev = v;
                }
            }
        }
    }

    return image;
}

uint qHash(const TexCoordCacheKey &key)
{
    return (qHash(key.distfield) << 13) | (key.glyph & 0x1FFF);
}

QHash<QString, DistanceFieldFontAtlas *> DistanceFieldFontAtlas::m_atlases;
QHash<TexCoordCacheKey, DistanceFieldFontAtlas::TexCoord> DistanceFieldFontAtlas::m_texCoords;
QHash<QString, bool> DistanceFieldFontAtlas::m_distfield_availability;
QHash<QString, QSGTextureRef> DistanceFieldFontAtlas::m_textures;

static QString fontKey(const QFont &font)
{
    QString key;

    QFontEngine *fontEngine = QFontPrivate::get(font)->engineForScript(QUnicodeTables::Common);
    if (fontEngine->type() == QFontEngine::Multi) {
        QFontEngineMulti *fem = static_cast<QFontEngineMulti *>(fontEngine);
        fontEngine = fem->engine(0);
    }

    key = fontEngine->fontDef.family;
    key.remove(QLatin1String(" "));
    QString italic = font.italic() ? QLatin1String("i") : QLatin1String("");
    QString bold = font.weight() > QFont::Normal ? QLatin1String("b") : QLatin1String("");
    key += bold + italic + QString::number(fontEngine->fontDef.pixelSize);

    return key;
}

DistanceFieldFontAtlas *DistanceFieldFontAtlas::get(const QFont &font)
{
    QString key = fontKey(font);
    QHash<QString, DistanceFieldFontAtlas *>::iterator atlas = m_atlases.find(key);
    if (atlas == m_atlases.end())
        atlas = m_atlases.insert(key, new DistanceFieldFontAtlas(font));

    return atlas.value();
}

DistanceFieldFontAtlas::DistanceFieldFontAtlas(const QFont &font)
{
    m_font = font;
    m_fontEngine = QFontPrivate::get(m_font)->engineForScript(QUnicodeTables::Common);
    if (m_fontEngine->type() == QFontEngine::Multi) {
        QFontEngineMulti *fem = static_cast<QFontEngineMulti *>(m_fontEngine);
        m_fontEngine = fem->engine(0);
    }
    qt_disableFontHinting(m_font);

    QFont referenceFont = m_font;
    referenceFont.setPixelSize(QT_DISTANCEFIELD_BASEFONTSIZE);
    qt_disableFontHinting(referenceFont);
    m_referenceFontEngine = QFontPrivate::get(referenceFont)->engineForScript(QUnicodeTables::Common);
    if (m_referenceFontEngine->type() == QFontEngine::Multi) {
        QFontEngineMulti *fem = static_cast<QFontEngineMulti *>(m_referenceFontEngine);
        m_referenceFontEngine = fem->engine(0);
    }

    QString basename = m_fontEngine->fontDef.family;
    basename.remove(QLatin1String(" "));
    QString italic = m_font.italic() ? QLatin1String("i") : QLatin1String("");
    QString bold = m_font.weight() > QFont::Normal ? QLatin1String("b") : QLatin1String("");
    m_distanceFieldFileName = basename + bold + italic + QLatin1String(".png");

    m_glyphCount = m_fontEngine->glyphCount();
    m_glyphMetricMargin = QT_DISTANCEFIELD_MARGIN / qreal(QT_DISTANCEFIELD_SCALE) * scaleRatioFromRefSize();
    m_glyphTexMargin = QT_DISTANCEFIELD_MARGIN / qreal(QT_DISTANCEFIELD_SCALE);
}

bool DistanceFieldFontAtlas::distanceFieldAvailable() const
{
    QHash<QString, bool>::iterator available = m_distfield_availability.find(m_distanceFieldFileName);
    if (available == m_distfield_availability.end()) {
        QString distFieldFile = QString(QLatin1String("%1/%2")).arg(distanceFieldDir()).arg(m_distanceFieldFileName);
        QFileInfo distFieldInfo(distFieldFile);
        bool exists = distFieldInfo.exists();
        if (!exists)
            qWarning("Warning: distance-field rendering '%s' does not exist", distFieldFile.toLatin1().constData());
        available = m_distfield_availability.insert(m_distanceFieldFileName, exists);
    }
    return available.value();
}

QSGTextureRef DistanceFieldFontAtlas::texture()
{
    if (!distanceFieldAvailable())
        return QSGTextureRef();

    QHash<QString, QSGTextureRef>::iterator tex = m_textures.find(m_distanceFieldFileName);
    if (tex == m_textures.end()) {
        QImage distFieldImage = distanceFieldAtlas();
        tex = m_textures.insert(m_distanceFieldFileName, uploadDistanceField(distFieldImage));
    }

    return tex.value();
}

QSize DistanceFieldFontAtlas::atlasSize() const
{
    if (!m_size.isValid()) {
        const int texWidth = QT_DISTANCEFIELD_TEXTURESIZE;
        const int texHeight = ((glyphCount() * QT_DISTANCEFIELD_TILESIZE) / texWidth + 1) * QT_DISTANCEFIELD_TILESIZE;
        m_size = QSize(texWidth, texHeight);
    }
    return m_size;
}

DistanceFieldFontAtlas::Metrics DistanceFieldFontAtlas::glyphMetrics(glyph_t glyph)
{
    QHash<glyph_t, Metrics>::iterator metric = m_metrics.find(glyph);
    if (metric == m_metrics.end()) {
        //XXX yoann temporary (and slow) solution to get metrics in float
        QPainterPath path;
        QFixedPoint p;
        m_fontEngine->addGlyphsToPath(&glyph, &p, 1, &path, 0);

        Metrics m;
        m.width = path.boundingRect().width();
        m.height = path.boundingRect().height();
        m.baselineX = path.boundingRect().x();
        m.baselineY = -path.boundingRect().y();

        metric = m_metrics.insert(glyph, m);
    }

    if (scaleRatioFromRefSize() <= QT_DISTANCEFIELD_MARGIN_THRESHOLD) {
        Metrics m = metric.value();
        m.width += m_glyphMetricMargin * 2;
        m.height += m_glyphMetricMargin * 2;
        m.baselineX -= m_glyphMetricMargin;
        m.baselineY += m_glyphMetricMargin;
        return m;
    }

    return metric.value();
}

DistanceFieldFontAtlas::TexCoord DistanceFieldFontAtlas::glyphTexCoord(glyph_t glyph)
{
    if (glyph >= glyphCount())
        qWarning("Warning: distance-field glyph is not available with index %d", glyph);

    TexCoordCacheKey key(distanceFieldFileName(), glyph);
    QHash<TexCoordCacheKey, TexCoord>::iterator texCoord = m_texCoords.find(key);
    if (texCoord == m_texCoords.end()) {
        const QSize texSize = atlasSize();

        //XXX yoann temporary (and slow) solution to get metrics in float
        QPainterPath path;
        QFixedPoint p;
        m_referenceFontEngine->addGlyphsToPath(&glyph, &p, 1, &path, 0);

        TexCoord c;
        c.xMargin = QT_DISTANCEFIELD_RADIUS / qreal(QT_DISTANCEFIELD_SCALE) / texSize.width();
        c.yMargin = QT_DISTANCEFIELD_RADIUS / qreal(QT_DISTANCEFIELD_SCALE) / texSize.height();
        c.x = ((glyph * QT_DISTANCEFIELD_TILESIZE) % texSize.width()) / qreal(texSize.width());
        c.y = ((glyph * QT_DISTANCEFIELD_TILESIZE) / texSize.width()) * QT_DISTANCEFIELD_TILESIZE / qreal(texSize.height());
        c.width = path.boundingRect().width() / qreal(texSize.width());
        c.height = path.boundingRect().height() / qreal(texSize.height());

        texCoord = m_texCoords.insert(key, c);
    }

    if (scaleRatioFromRefSize() <= QT_DISTANCEFIELD_MARGIN_THRESHOLD) {
        TexCoord c = texCoord.value();
        c.xMargin -= m_glyphTexMargin / atlasSize().width();
        c.yMargin -= m_glyphTexMargin / atlasSize().height();
        c.width += (m_glyphTexMargin * 2) / qreal(atlasSize().width());
        c.height += (m_glyphTexMargin * 2) / qreal(atlasSize().height());
        return c;
    }

    return texCoord.value();
}

QImage DistanceFieldFontAtlas::renderDistanceFieldGlyph(glyph_t glyph) const
{
    QFont renderFont = m_font;
    renderFont.setPixelSize(QT_DISTANCEFIELD_BASEFONTSIZE * QT_DISTANCEFIELD_SCALE);
    qt_disableFontHinting(renderFont);

    QFontEngine *fontEngine = QFontPrivate::get(renderFont)->engineForScript(QUnicodeTables::Common);
    if (fontEngine->type() == QFontEngine::Multi) {
        QFontEngineMulti *fem = static_cast<QFontEngineMulti *>(fontEngine);
        fontEngine = fem->engine(0);
    }

    QPainterPath path;
    QFixedPoint pt;
    fontEngine->addGlyphsToPath(&glyph, &pt, 1, &path, 0);
    path.translate(-path.boundingRect().topLeft());

    QImage im = makeDistanceField(path, QT_DISTANCEFIELD_RADIUS / qreal(QT_DISTANCEFIELD_SCALE));
    return im;
}

qreal DistanceFieldFontAtlas::scaleRatioFromRefSize() const
{
    return m_fontEngine->fontDef.pixelSize / QT_DISTANCEFIELD_BASEFONTSIZE;
}

QImage DistanceFieldFontAtlas::distanceFieldAtlas() const
{
    if (!distanceFieldAvailable())
        return QImage();

    return QImage(QString(QLatin1String("%1/%2")).arg(distanceFieldDir()).arg(m_distanceFieldFileName));
}

bool DistanceFieldFontAtlas::useDistanceFieldForFont(const QFont &font)
{
    static QStringList args = qApp->arguments();
    if (args.contains(QLatin1String("--distancefield-text"))) {
        return DistanceFieldFontAtlas::get(font)->distanceFieldAvailable();
    }
    return false;
}

static void convert_to_Format_Alpha(QImage *image)
{
    const int width = image->width();
    const int height = image->height();
    uchar *data = image->bits();

    for (int i = 0; i < height; ++i) {
        uchar *o = data + i * width;
        for (int x = 0; x < width; ++x)
            o[x] = (uchar)qAlpha(image->pixel(x, i));
    }
}

QSGTextureRef DistanceFieldFontAtlas::uploadDistanceField(const QImage &image)
{
    Q_ASSERT(!image.isNull());

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    QImage i = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    convert_to_Format_Alpha(&i);

    // We only need to store the alpha component
#ifdef QT_OPENGL_ES
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, i.width(), i.height(), 0, GL_ALPHA, GL_UNSIGNED_BYTE, i.constBits());
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA8, i.width(), i.height(), 0, GL_ALPHA, GL_UNSIGNED_BYTE, i.constBits());
#endif

    GLuint error = glGetError();
    if (error != GL_NO_ERROR) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &id);
        return QSGTextureRef();
    }

    QSGTexture *texture = new QSGTexture;
    texture->setTextureId(id);
    texture->setTextureSize(image.size());
    texture->setAlphaChannel(image.hasAlphaChannel());
    texture->setStatus(QSGTexture::Ready);

    QSGTextureRef ref(texture);
    return ref;
}

QString DistanceFieldFontAtlas::distanceFieldDir() const
{
    static QString distfieldpath = QString::fromLocal8Bit(qgetenv("QT_QML_DISTFIELDDIR"));
    if (distfieldpath.isEmpty()) {
#ifndef QT_NO_SETTINGS
        distfieldpath = QLibraryInfo::location(QLibraryInfo::LibrariesPath);
        distfieldpath += QLatin1String("/fonts/distancefields");
#endif
    }

    return distfieldpath;
}

QString DistanceFieldFontAtlas::distanceFieldFileName() const
{
    return m_distanceFieldFileName;
}

int DistanceFieldFontAtlas::glyphCount() const
{
    return m_glyphCount;
}
