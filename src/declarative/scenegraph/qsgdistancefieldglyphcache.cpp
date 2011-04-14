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

#include "qsgdistancefieldglyphcache_p.h"

#include <qmath.h>
#include <private/qtriangulator_p.h>
#include <private/qdeclarativeglobal_p.h>
#include <qglshaderprogram.h>
#include <private/qglengineshadersource_p.h>
#include <private/qsgcontext_p.h>
#include <private/qrawfont_p.h>
#include <qglfunctions.h>
#include <qglyphs.h>
#include <qrawfont.h>

QT_BEGIN_NAMESPACE

#define QT_DISTANCEFIELD_DEFAULT_BASEFONTSIZE 54
#define QT_DISTANCEFIELD_DEFAULT_TILESIZE 64
#define QT_DISTANCEFIELD_DEFAULT_SCALE 16
#define QT_DISTANCEFIELD_DEFAULT_RADIUS 80
#define QT_DISTANCEFIELD_HIGHGLYPHCOUNT 2000

#define QT_DISTANCEFIELD_BASEFONTSIZE \
    (m_textureData->doubleGlyphResolution ? QT_DISTANCEFIELD_DEFAULT_BASEFONTSIZE * 2 : \
                                           QT_DISTANCEFIELD_DEFAULT_BASEFONTSIZE)
#define QT_DISTANCEFIELD_TILESIZE \
    (m_textureData->doubleGlyphResolution ? QT_DISTANCEFIELD_DEFAULT_TILESIZE * 2 : \
                                           QT_DISTANCEFIELD_DEFAULT_TILESIZE)
#define QT_DISTANCEFIELD_SCALE \
    (m_textureData->doubleGlyphResolution ? QT_DISTANCEFIELD_DEFAULT_SCALE / 2 : \
                                           QT_DISTANCEFIELD_DEFAULT_SCALE)
#define QT_DISTANCEFIELD_RADIUS \
    (m_textureData->doubleGlyphResolution ? QT_DISTANCEFIELD_DEFAULT_RADIUS / 2 : \
                                           QT_DISTANCEFIELD_DEFAULT_RADIUS)

static inline int qt_next_power_of_two(int v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    ++v;
    return v;
}

struct DFPoint
{
    float x, y;
};

struct DFVertex
{
    DFPoint p;
    float d;
};

static void drawRectangle(float *bits, int width, int height, const DFVertex *v1, const DFVertex *v2, const DFVertex *v3, const DFVertex *v4)
{
    float minY = qMin(qMin(v1->p.y, v2->p.y), qMin(v3->p.y, v4->p.y));
    if (v2->p.y == minY) {
        const DFVertex *tmp = v1;
        v1 = v2;
        v2 = v3;
        v3 = v4;
        v4 = tmp;
    } else if (v3->p.y == minY) {
        const DFVertex *tmp1 = v1;
        const DFVertex *tmp2 = v2;
        v1 = v3;
        v2 = v4;
        v3 = tmp1;
        v4 = tmp2;
    } else if (v4->p.y == minY) {
        const DFVertex *tmp = v4;
        v4 = v3;
        v3 = v2;
        v2 = v1;
        v1 = tmp;
    }

    /*
       v1
      /  \
     v4  v2
      \  /
       v3
    */

    int fromY = qMax(0, qCeil(v1->p.y));
    int midY1 = qMin(height, qCeil(qMin(v2->p.y, v4->p.y)));
    int midY2 = qMin(height, qCeil(qMax(v2->p.y, v4->p.y)));
    int toY = qMin(height, qCeil(v3->p.y));

    if (toY <= fromY)
        return;

    bits += width * fromY;
    int y = fromY;

    float leftDx = (v4->p.x - v1->p.x) / (v4->p.y - v1->p.y);
    float leftDd = (v4->d - v1->d) / (v4->p.y - v1->p.y);
    float leftX = v1->p.x + (fromY - v1->p.y) * leftDx;
    float leftD = v1->d + (fromY - v1->p.y) * leftDd;

    float rightDx = (v2->p.x - v1->p.x) / (v2->p.y - v1->p.y);
    float rightDd = (v2->d - v1->d) / (v2->p.y - v1->p.y);
    float rightX = v1->p.x + (fromY - v1->p.y) * rightDx;
    float rightD = v1->d + (fromY - v1->p.y) * rightDd;

    float dd = ((v2->d - v1->d) * (v3->p.y - v1->p.y) - (v2->p.y - v1->p.y) * (v3->d - v1->d))
             / ((v2->p.x - v1->p.x) * (v3->p.y - v1->p.y) - (v2->p.y - v1->p.y) * (v3->p.x - v1->p.x));

    for (; y < midY1; ++y, leftX += leftDx, leftD += leftDd, rightX += rightDx, rightD += rightDd, bits += width) {
        int fromX = qMax(0, qCeil(leftX));
        int toX = qMin(width, qCeil(rightX));
        if (toX <= fromX)
            continue;
        float d = leftD + (fromX - leftX) * dd;
        for (int x = fromX; x < toX; ++x, d += dd) {
            if (abs(d) < abs(bits[x]))
                bits[x] = d;
        }
    }

    if (midY1 == toY)
        return;

    if (v2->p.y > v4->p.y) {
        // Long right edge.
        leftDx = (v3->p.x - v4->p.x) / (v3->p.y - v4->p.y);
        leftDd = (v3->d - v4->d) / (v3->p.y - v4->p.y);
        leftX = v4->p.x + (midY1 - v4->p.y) * leftDx;
        leftD = v4->d + (midY1 - v4->p.y) * leftDd;
    } else {
        // Long left edge.
        rightDx = (v3->p.x - v2->p.x) / (v3->p.y - v2->p.y);
        rightDd = (v3->d - v2->d) / (v3->p.y - v2->p.y);
        rightX = v2->p.x + (midY1 - v2->p.y) * rightDx;
        rightD = v2->d + (midY1 - v2->p.y) * rightDd;
    }

    for (; y < midY2; ++y, leftX += leftDx, leftD += leftDd, rightX += rightDx, rightD += rightDd, bits += width) {
        int fromX = qMax(0, qCeil(leftX));
        int toX = qMin(width, qCeil(rightX));
        if (toX <= fromX)
            continue;
        float d = leftD + (fromX - leftX) * dd;
        for (int x = fromX; x < toX; ++x, d += dd) {
            if (abs(d) < abs(bits[x]))
                bits[x] = d;
        }
    }

    if (midY2 == toY)
        return;

    if (v2->p.y > v4->p.y) {
        // Long left edge.
        rightDx = (v3->p.x - v2->p.x) / (v3->p.y - v2->p.y);
        rightDd = (v3->d - v2->d) / (v3->p.y - v2->p.y);
        rightX = v2->p.x + (midY2 - v2->p.y) * rightDx;
        rightD = v2->d + (midY2 - v2->p.y) * rightDd;
    } else {
        // Long right edge.
        leftDx = (v3->p.x - v4->p.x) / (v3->p.y - v4->p.y);
        leftDd = (v3->d - v4->d) / (v3->p.y - v4->p.y);
        leftX = v4->p.x + (midY2 - v4->p.y) * leftDx;
        leftD = v4->d + (midY2 - v4->p.y) * leftDd;
    }

    for (; y < toY; ++y, leftX += leftDx, leftD += leftDd, rightX += rightDx, rightD += rightDd, bits += width) {
        int fromX = qMax(0, qCeil(leftX));
        int toX = qMin(width, qCeil(rightX));
        if (toX <= fromX)
            continue;
        float d = leftD + (fromX - leftX) * dd;
        for (int x = fromX; x < toX; ++x, d += dd) {
            if (abs(d) < abs(bits[x]))
                bits[x] = d;
        }
    }
}

static void drawTriangle(float *bits, int width, int height, const DFVertex *v1, const DFVertex *v2, const DFVertex *v3)
{
    float minY = qMin(qMin(v1->p.y, v2->p.y), v3->p.y);
    if (v2->p.y == minY) {
        const DFVertex *tmp = v1;
        v1 = v2;
        v2 = v3;
        v3 = tmp;
    } else if (v3->p.y == minY) {
        const DFVertex *tmp = v3;
        v3 = v2;
        v2 = v1;
        v1 = tmp;
    }

    //   v1
    //  /  \
    // v3--v2

    int fromY = qMax(0, qCeil(v1->p.y));
    int midY = qMin(height, qCeil(qMin(v2->p.y, v3->p.y)));
    int toY = qMin(height, qCeil(qMax(v2->p.y, v3->p.y)));

    if (toY <= fromY)
        return;

    float leftDx = (v3->p.x - v1->p.x) / (v3->p.y - v1->p.y);
    float leftDd = (v3->d - v1->d) / (v3->p.y - v1->p.y);
    float leftX = v1->p.x + (fromY - v1->p.y) * leftDx;
    float leftD = v1->d + (fromY - v1->p.y) * leftDd;

    float rightDx = (v2->p.x - v1->p.x) / (v2->p.y - v1->p.y);
    float rightDd = (v2->d - v1->d) / (v2->p.y - v1->p.y);
    float rightX = v1->p.x + (fromY - v1->p.y) * rightDx;
    float rightD = v1->d + (fromY - v1->p.y) * rightDd;

    float dd = ((v2->d - v1->d) * (v3->p.y - v1->p.y) - (v2->p.y - v1->p.y) * (v3->d - v1->d))
             / ((v2->p.x - v1->p.x) * (v3->p.y - v1->p.y) - (v2->p.y - v1->p.y) * (v3->p.x - v1->p.x));

    bits += width * fromY;
    int y = fromY;
    for (; y < midY; ++y, leftX += leftDx, leftD += leftDd, rightX += rightDx, rightD += rightDd, bits += width) {
        int fromX = qMax(0, qCeil(leftX));
        int toX = qMin(width, qCeil(rightX));
        if (toX <= fromX)
            continue;
        float d = leftD + (fromX - leftX) * dd;
        for (int x = fromX; x < toX; ++x, d += dd) {
            if (abs(d) < abs(bits[x]))
                bits[x] = d;
        }
    }

    if (midY == toY)
        return;

    if (v2->p.y > v3->p.y) {
        // Long right edge.
        leftDx = (v2->p.x - v3->p.x) / (v2->p.y - v3->p.y);
        leftDd = (v2->d - v3->d) / (v2->p.y - v3->p.y);
        leftX = v3->p.x + (midY - v3->p.y) * leftDx;
        leftD = v3->d + (midY - v3->p.y) * leftDd;
    } else {
        // Long left edge.
        rightDx = (v3->p.x - v2->p.x) / (v3->p.y - v2->p.y);
        rightDd = (v3->d - v2->d) / (v3->p.y - v2->p.y);
        rightX = v2->p.x + (midY - v2->p.y) * rightDx;
        rightD = v2->d + (midY - v2->p.y) * rightDd;
    }

    for (; y < toY; ++y, leftX += leftDx, leftD += leftDd, rightX += rightDx, rightD += rightDd, bits += width) {
        int fromX = qMax(0, qCeil(leftX));
        int toX = qMin(width, qCeil(rightX));
        if (toX <= fromX)
            continue;
        float d = leftD + (fromX - leftX) * dd;
        for (int x = fromX; x < toX; ++x, d += dd) {
            if (abs(d) < abs(bits[x]))
                bits[x] = d;
        }
    }
}

static QImage makeDistanceField(int imgSize, const QPainterPath &path, int dfScale, float offs)
{
    QImage image(imgSize, imgSize, QImage::Format_ARGB32_Premultiplied);

    if (path.isEmpty()) {
        image.fill(0);
        return image;
    }

    QPolylineSet polys = qPolyline(path);

    union Pacific {
        float value;
        QRgb color;
    };
    Pacific interior;
    Pacific exterior;
    interior.value = 127;
    exterior.value = -127;

    image.fill(exterior.color);

    QPainter p(&image);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.translate(offs, offs);
    p.scale(1 / qreal(dfScale), 1 / qreal(dfScale));
    p.fillPath(path, QColor::fromRgba(interior.color));
    p.end();

    float *bits = (float *)image.bits();
    const float angleStep = 15 * 3.141592653589793238f / 180;
    const DFPoint rotation = { cos(angleStep), sin(angleStep) };

    bool isShortData = polys.indices.type() == QVertexIndexVector::UnsignedShort;
    const void *indices = polys.indices.data();
    int index = 0;
    QVector<DFPoint> normals;
    QVector<DFVertex> vertices;
    normals.reserve(polys.vertices.count());
    vertices.reserve(polys.vertices.count());

    while (index < polys.indices.size()) {
        normals.clear();
        vertices.clear();

        // Find end of polygon.
        int end = index;
        if (isShortData) {
            while (((quint16 *)indices)[end] != quint16(-1))
                ++end;
        } else {
            while (((quint32 *)indices)[end] != quint32(-1))
                ++end;
        }

        // Calculate vertex normals.
        for (int next = index, prev = end - 1; next < end; prev = next++) {
            quint32 fromVertexIndex = isShortData ? (quint32)((quint16 *)indices)[prev] : ((quint32 *)indices)[prev];
            quint32 toVertexIndex = isShortData ? (quint32)((quint16 *)indices)[next] : ((quint32 *)indices)[next];
            const qreal *from = &polys.vertices.at(fromVertexIndex * 2);
            const qreal *to = &polys.vertices.at(toVertexIndex * 2);
            DFPoint n;
            n.x = float(to[1] - from[1]);
            n.y = float(from[0] - to[0]);
            if (n.x == 0 && n.y == 0)
                continue;
            float scale = offs / sqrt(n.x * n.x + n.y * n.y);
            n.x *= scale;
            n.y *= scale;
            normals.append(n);

            DFVertex v;
            v.p.x = float(to[0] / dfScale) + offs - 0.5f;
            v.p.y = float(to[1] / dfScale) + offs - 0.5f;
            v.d = 0.0f;
            vertices.append(v);
        }

        QVector<bool> isConvex(normals.count());
        for (int next = 0, prev = normals.count() - 1; next < normals.count(); prev = next++)
            isConvex[prev] = (normals.at(prev).x * normals.at(next).y - normals.at(prev).y * normals.at(next).x > 0);

        // Draw quads.
        for (int next = 0, prev = normals.count() - 1; next < normals.count(); prev = next++) {
            DFPoint n = normals.at(next);
            DFVertex intPrev = vertices.at(prev);
            DFVertex extPrev = vertices.at(prev);
            DFVertex intNext = vertices.at(next);
            DFVertex extNext = vertices.at(next);

            extPrev.p.x += n.x;
            extPrev.p.y += n.y;
            intPrev.p.x -= n.x;
            intPrev.p.y -= n.y;
            extNext.p.x += n.x;
            extNext.p.y += n.y;
            intNext.p.x -= n.x;
            intNext.p.y -= n.y;
            extPrev.d = 127;
            extNext.d = 127;
            intPrev.d = -127;
            intNext.d = -127;

            drawRectangle(bits, image.width(), image.height(),
                          &vertices.at(prev), &extPrev, &extNext, &vertices.at(next));

            drawRectangle(bits, image.width(), image.height(),
                          &intPrev, &vertices.at(prev), &vertices.at(next), &intNext);

            if (isConvex.at(prev)) {
                DFVertex v = extPrev;
                for (;;) {
                    DFPoint rn = { n.x * rotation.x + n.y * rotation.y,
                                 n.y * rotation.x - n.x * rotation.y };
                    n = rn;
                    if (n.x * normals.at(prev).y - n.y * normals.at(prev).x >= -0.001) {
                        v.p.x = vertices.at(prev).p.x + normals.at(prev).x;
                        v.p.y = vertices.at(prev).p.y + normals.at(prev).y;
                        drawTriangle(bits, image.width(), image.height(), &vertices.at(prev), &v, &extPrev);
                        break;
                    }

                    v.p.x = vertices.at(prev).p.x + n.x;
                    v.p.y = vertices.at(prev).p.y + n.y;
                    drawTriangle(bits, image.width(), image.height(), &vertices.at(prev), &v, &extPrev);
                    extPrev = v;
                }
            } else {
                DFVertex v = intPrev;
                for (;;) {
                    DFPoint rn = { n.x * rotation.x - n.y * rotation.y,
                                 n.y * rotation.x + n.x * rotation.y };
                    n = rn;
                    if (n.x * normals.at(prev).y - n.y * normals.at(prev).x <= 0.001) {
                        v.p.x = vertices.at(prev).p.x - normals.at(prev).x;
                        v.p.y = vertices.at(prev).p.y - normals.at(prev).y;
                        drawTriangle(bits, image.width(), image.height(), &vertices.at(prev), &intPrev, &v);
                        break;
                    }

                    v.p.x = vertices.at(prev).p.x - n.x;
                    v.p.y = vertices.at(prev).p.y - n.y;
                    drawTriangle(bits, image.width(), image.height(), &vertices.at(prev), &intPrev, &v);
                    intPrev = v;
                }
            }
        }
        index = end + 1;
    }

    for (int y = 0; y < image.height(); ++y) {
        QRgb *iLine = (QRgb *)image.scanLine(y);
        float *fLine = (float *)iLine;
        for (int x = 0; x < image.width(); ++x)
            iLine[x] = QRgb(fLine[x] + 127.5) << 24;
    }

    return image;
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

static bool fontHasNarrowOutlines(const QRawFont &f)
{
    return true;
    QRawFont font = f;
    font.setPixelSize(QT_DISTANCEFIELD_DEFAULT_BASEFONTSIZE);
    Q_ASSERT(font.isValid());

    QVector<quint32> glyphIndices = font.glyphIndexesForString(QLatin1String("O"));
    if (glyphIndices.size() < 1)
        return false;

    QImage im = font.alphaMapForGlyph(glyphIndices.at(0), QRawFont::PixelAntialiasing);
    if (im.isNull())
        return false;

    int minHThick = 999;
    int minVThick = 999;

    int thick = 0;
    bool in = false;
    int y = (im.height() + 1) / 2;
    for (int x = 0; x < im.width(); ++x) {
        int a = qAlpha(im.pixel(x, y));
        if (a > 127) {
            in = true;
            ++thick;
        } else if (in) {
            in = false;
            minHThick = qMin(minHThick, thick);
            thick = 0;
        }
    }

    thick = 0;
    in = false;
    int x = (im.width() + 1) / 2;
    for (int y = 0; y < im.height(); ++y) {
        int a = qAlpha(im.pixel(x, y));
        if (a > 127) {
            in = true;
            ++thick;
        } else if (in) {
            in = false;
            minVThick = qMin(minVThick, thick);
            thick = 0;
        }
    }

    return minHThick == 1 || minVThick == 1;
}

DEFINE_BOOL_CONFIG_OPTION(disableDistanceField, QML_DISABLE_DISTANCEFIELD)

QHash<QString, QSGDistanceFieldGlyphCache *> QSGDistanceFieldGlyphCache::m_caches;
QHash<QString, QGLContextGroupResource<QSGDistanceFieldGlyphCache::DistanceFieldTextureData> > QSGDistanceFieldGlyphCache::m_textures_data;

static QString fontKey(const QRawFont &font)
{
    QString key;

    key = font.familyName();
    key.remove(QLatin1String(" "));
    QString italic = font.style() == QFont::StyleItalic ? QLatin1String("i") : QLatin1String("");
    QString bold = font.weight() > QFont::Normal ? QLatin1String("b") : QLatin1String("");
    key += bold + italic + QString::number(qreal(font.pixelSize()));

    return key;
}

QSGDistanceFieldGlyphCache *QSGDistanceFieldGlyphCache::get(const QGLContext *ctx, const QRawFont &font)
{
    QString key = QString::number(long(ctx), 16) + fontKey(font);
    QHash<QString, QSGDistanceFieldGlyphCache *>::iterator atlas = m_caches.find(key);
    if (atlas == m_caches.end())
        atlas = m_caches.insert(key, new QSGDistanceFieldGlyphCache(ctx, font));

    return atlas.value();
}

QSGDistanceFieldGlyphCache::DistanceFieldTextureData *QSGDistanceFieldGlyphCache::textureData()
{
    return m_textures_data[m_distanceFieldKey].value(ctx);
}

QSGDistanceFieldGlyphCache::QSGDistanceFieldGlyphCache(const QGLContext *c, const QRawFont &font)
    : QObject()
    , m_maxTextureSize(0)
    , ctx(c)
    , m_blitProgram(0)
{
    Q_ASSERT(font.isValid());
    m_font = font;

    QString basename = m_font.familyName();
    basename.remove(QLatin1String(" "));
    QString italic = m_font.style() == QFont::StyleItalic ? QLatin1String("i") : QLatin1String("");
    QString bold = m_font.weight() > QFont::Normal ? QLatin1String("b") : QLatin1String("");
    m_distanceFieldKey = basename + bold + italic;
    m_textureData = textureData();

    QRawFontPrivate *fontD = QRawFontPrivate::get(m_font);
    m_glyphCount = fontD->fontEngine->glyphCount();

    m_textureData->doubleGlyphResolution = fontHasNarrowOutlines(font) && m_glyphCount < QT_DISTANCEFIELD_HIGHGLYPHCOUNT;

    m_referenceFont = m_font;
    m_referenceFont.setPixelSize(QT_DISTANCEFIELD_BASEFONTSIZE);
    Q_ASSERT(m_referenceFont.isValid());

    m_vertexCoordinateArray[0] = -1.0f;
    m_vertexCoordinateArray[1] = -1.0f;
    m_vertexCoordinateArray[2] =  1.0f;
    m_vertexCoordinateArray[3] = -1.0f;
    m_vertexCoordinateArray[4] =  1.0f;
    m_vertexCoordinateArray[5] =  1.0f;
    m_vertexCoordinateArray[6] = -1.0f;
    m_vertexCoordinateArray[7] =  1.0f;

    m_textureCoordinateArray[0] = 0.0f;
    m_textureCoordinateArray[1] = 0.0f;
    m_textureCoordinateArray[2] = 1.0f;
    m_textureCoordinateArray[3] = 0.0f;
    m_textureCoordinateArray[4] = 1.0f;
    m_textureCoordinateArray[5] = 1.0f;
    m_textureCoordinateArray[6] = 0.0f;
    m_textureCoordinateArray[7] = 1.0f;

    connect(QGLSignalProxy::instance(), SIGNAL(aboutToDestroyContext(const QGLContext*)),
            this, SLOT(onContextDestroyed(const QGLContext*)));
}

QSGDistanceFieldGlyphCache::~QSGDistanceFieldGlyphCache()
{
    delete m_blitProgram;
}

void QSGDistanceFieldGlyphCache::onContextDestroyed(const QGLContext *context)
{
    if (context != ctx)
        return;

    QString key = QString::number(long(context), 16) + fontKey(m_font);
    m_caches.remove(key);
    deleteLater();
}

GLuint QSGDistanceFieldGlyphCache::texture()
{
    return m_textureData->texture;
}

QSize QSGDistanceFieldGlyphCache::textureSize() const
{
    return m_textureData->size;
}

int QSGDistanceFieldGlyphCache::maxTextureSize() const
{
    if (!m_maxTextureSize)
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxTextureSize);
    return m_maxTextureSize;
}

QSGDistanceFieldGlyphCache::Metrics QSGDistanceFieldGlyphCache::glyphMetrics(glyph_t glyph)
{
    QHash<glyph_t, Metrics>::iterator metric = m_metrics.find(glyph);
    if (metric == m_metrics.end()) {
        QPainterPath path = m_font.pathForGlyph(glyph);

        Metrics m;
        m.width = path.boundingRect().width();
        m.height = path.boundingRect().height();
        m.baselineX = path.boundingRect().x();
        m.baselineY = -path.boundingRect().y();

        metric = m_metrics.insert(glyph, m);
    }

    return metric.value();
}

QSGDistanceFieldGlyphCache::TexCoord QSGDistanceFieldGlyphCache::glyphTexCoord(glyph_t glyph)
{
    return m_textureData->texCoords.value(glyph);
}

QImage QSGDistanceFieldGlyphCache::renderDistanceFieldGlyph(glyph_t glyph) const
{
    QRawFont renderFont = m_font;
    renderFont.setPixelSize(QT_DISTANCEFIELD_BASEFONTSIZE * QT_DISTANCEFIELD_SCALE);

    QPainterPath path = renderFont.pathForGlyph(glyph);
    path.translate(-path.boundingRect().topLeft());
    path.setFillRule(Qt::WindingFill);

    QImage im = makeDistanceField(QT_DISTANCEFIELD_TILESIZE,
                                  path,
                                  QT_DISTANCEFIELD_SCALE,
                                  QT_DISTANCEFIELD_RADIUS / qreal(QT_DISTANCEFIELD_SCALE));
    return im;
}

qreal QSGDistanceFieldGlyphCache::fontScale() const
{
    return qreal(m_font.pixelSize()) / QT_DISTANCEFIELD_BASEFONTSIZE;
}

int QSGDistanceFieldGlyphCache::distanceFieldRadius() const
{
    return QT_DISTANCEFIELD_DEFAULT_RADIUS / QT_DISTANCEFIELD_SCALE;
}

void QSGDistanceFieldGlyphCache::populate(int count, const glyph_t *glyphs)
{
    for (int i = 0; i < count; ++i) {
        glyph_t glyphIndex = glyphs[i];
        if (glyphIndex >= glyphCount()) {
            qWarning("Warning: distance-field glyph is not available with index %d", glyphIndex);
            continue;
        }

        if (++m_textureData->glyphRefCount[glyphIndex] == 1)
            m_textureData->unusedGlyphs.remove(glyphIndex);

        if (m_textureData->texCoords.contains(glyphIndex)
                || (cacheIsFull() && m_textureData->unusedGlyphs.isEmpty()))
            continue;

        QPainterPath path = m_referenceFont.pathForGlyph(glyphIndex);
        if (path.isEmpty()) {
            m_textureData->texCoords.insert(glyphIndex, TexCoord());
            continue;
        }

        TexCoord c;
        c.xMargin = QT_DISTANCEFIELD_RADIUS / qreal(QT_DISTANCEFIELD_SCALE);
        c.yMargin = QT_DISTANCEFIELD_RADIUS / qreal(QT_DISTANCEFIELD_SCALE);
        c.x = m_textureData->currX;
        c.y = m_textureData->currY;
        c.width = path.boundingRect().width();
        c.height = path.boundingRect().height();

        if (!cacheIsFull()) {
            m_textureData->currX += QT_DISTANCEFIELD_TILESIZE;
            if (m_textureData->currX >= maxTextureSize()) {
                m_textureData->currX = 0;
                m_textureData->currY += QT_DISTANCEFIELD_TILESIZE;
            }
        } else {
            // Recycle glyphs
            if (!m_textureData->unusedGlyphs.isEmpty()) {
                glyph_t unusedGlyph = *m_textureData->unusedGlyphs.constBegin();
                TexCoord unusedCoord = glyphTexCoord(unusedGlyph);
                c.x = unusedCoord.x;
                c.y = unusedCoord.y;
                m_textureData->unusedGlyphs.remove(unusedGlyph);
                m_textureData->texCoords.remove(unusedGlyph);
            }
        }

        if (c.y < maxTextureSize()) {
            m_textureData->texCoords.insert(glyphIndex, c);
            m_textureData->pendingGlyphs.insert(glyphIndex);
        }
    }
}

void QSGDistanceFieldGlyphCache::derefGlyphs(int count, const glyph_t *glyphs)
{
    for (int i = 0; i < count; ++i)
        if (--m_textureData->glyphRefCount[glyphs[i]] == 0 && !glyphTexCoord(glyphs[i]).isNull())
            m_textureData->unusedGlyphs.insert(glyphs[i]);
}

void QSGDistanceFieldGlyphCache::createTexture(int width, int height)
{
    if (ctx->d_ptr->workaround_brokenFBOReadBack && m_textureData->image.isNull())
        m_textureData->image = QImage(width, height, QImage::Format_ARGB32_Premultiplied);

    while (glGetError() != GL_NO_ERROR) { }

    glGenTextures(1, &m_textureData->texture);
    glBindTexture(GL_TEXTURE_2D, m_textureData->texture);

    QVarLengthArray<uchar> data(width * height);
    for (int i = 0; i < data.size(); ++i)
        data[i] = 0;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, &data[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    m_textureData->size = QSize(width, height);

    GLuint error = glGetError();
    if (error != GL_NO_ERROR) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &m_textureData->texture);
        m_textureData->texture = 0;
    }

}

void QSGDistanceFieldGlyphCache::resizeTexture(int width, int height)
{
    int oldWidth = m_textureData->size.width();
    int oldHeight = m_textureData->size.height();
    if (width == oldWidth && height == oldHeight)
        return;

    GLuint oldTexture = m_textureData->texture;
    createTexture(width, height);

    if (!oldTexture)
        return;

    if (ctx->d_ptr->workaround_brokenFBOReadBack) {
        m_textureData->image = m_textureData->image.copy(0, 0, width, height);
        QImage copy = m_textureData->image.copy(0, 0, oldWidth, oldHeight);
        convert_to_Format_Alpha(&copy);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, oldWidth, oldHeight, GL_ALPHA, GL_UNSIGNED_BYTE, copy.constBits());
        glDeleteTextures(1, &oldTexture);
        return;
    }

    if (!m_textureData->fbo)
        ctx->functions()->glGenFramebuffers(1, &m_textureData->fbo);
    ctx->functions()->glBindFramebuffer(GL_FRAMEBUFFER_EXT, m_textureData->fbo);

    GLuint tmp_texture;
    glGenTextures(1, &tmp_texture);
    glBindTexture(GL_TEXTURE_2D, tmp_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, oldWidth, oldHeight, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    ctx->functions()->glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                             GL_TEXTURE_2D, tmp_texture, 0);

    ctx->functions()->glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, oldTexture);

    // save current render states
    GLboolean stencilTestEnabled;
    GLboolean depthTestEnabled;
    GLboolean scissorTestEnabled;
    GLboolean blendEnabled;
    GLint viewport[4];
    glGetBooleanv(GL_STENCIL_TEST, &stencilTestEnabled);
    glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
    glGetBooleanv(GL_SCISSOR_TEST, &scissorTestEnabled);
    glGetBooleanv(GL_BLEND, &blendEnabled);
    glGetIntegerv(GL_VIEWPORT, &viewport[0]);

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_BLEND);

    glViewport(0, 0, oldWidth, oldHeight);

    if (m_blitProgram == 0) {
        m_blitProgram = new QGLShaderProgram;

        {
            QString source;
            source.append(QLatin1String(qglslMainWithTexCoordsVertexShader));
            source.append(QLatin1String(qglslUntransformedPositionVertexShader));

            QGLShader *vertexShader = new QGLShader(QGLShader::Vertex, m_blitProgram);
            vertexShader->compileSourceCode(source);

            m_blitProgram->addShader(vertexShader);
        }

        {
            QString source;
            source.append(QLatin1String(qglslMainFragmentShader));
            source.append(QLatin1String(qglslImageSrcFragmentShader));

            QGLShader *fragmentShader = new QGLShader(QGLShader::Fragment, m_blitProgram);
            fragmentShader->compileSourceCode(source);

            m_blitProgram->addShader(fragmentShader);
        }

        m_blitProgram->bindAttributeLocation("vertexCoordsArray", QT_VERTEX_COORDS_ATTR);
        m_blitProgram->bindAttributeLocation("textureCoordArray", QT_TEXTURE_COORDS_ATTR);

        m_blitProgram->link();
    }

    ctx->functions()->glVertexAttribPointer(QT_VERTEX_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, m_vertexCoordinateArray);
    ctx->functions()->glVertexAttribPointer(QT_TEXTURE_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, m_textureCoordinateArray);

    m_blitProgram->bind();
    m_blitProgram->enableAttributeArray(int(QT_VERTEX_COORDS_ATTR));
    m_blitProgram->enableAttributeArray(int(QT_TEXTURE_COORDS_ATTR));
    m_blitProgram->disableAttributeArray(int(QT_OPACITY_ATTR));
    m_blitProgram->setUniformValue("imageTexture", GLuint(0));

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindTexture(GL_TEXTURE_2D, m_textureData->texture);

    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, oldWidth, oldHeight);

    ctx->functions()->glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                                GL_RENDERBUFFER_EXT, 0);
    glDeleteTextures(1, &tmp_texture);
    glDeleteTextures(1, &oldTexture);

    ctx->functions()->glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

    // restore render states
    if (stencilTestEnabled)
        glEnable(GL_STENCIL_TEST);
    if (depthTestEnabled)
        glEnable(GL_DEPTH_TEST);
    if (scissorTestEnabled)
        glEnable(GL_SCISSOR_TEST);
    if (blendEnabled)
        glEnable(GL_BLEND);
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void QSGDistanceFieldGlyphCache::updateCache()
{
    if (m_textureData->pendingGlyphs.isEmpty())
        return;

    int requiredWidth = m_textureData->currY == 0 ? m_textureData->currX : maxTextureSize();
    int requiredHeight = qMin(maxTextureSize(), m_textureData->currY + QT_DISTANCEFIELD_TILESIZE);

    resizeTexture((requiredWidth), (requiredHeight));
    glBindTexture(GL_TEXTURE_2D, m_textureData->texture);

    QSet<glyph_t>::const_iterator i = m_textureData->pendingGlyphs.constBegin();
    for (; i != m_textureData->pendingGlyphs.constEnd(); ++i) {
        QImage glyph = renderDistanceFieldGlyph(*i);
        TexCoord c = m_textureData->texCoords.value(*i);

        if (ctx->d_ptr->workaround_brokenFBOReadBack) {
            QPainter p(&m_textureData->image);
            p.setCompositionMode(QPainter::CompositionMode_Source);
            p.drawImage(c.x, c.y, glyph);
            p.end();
        }

        convert_to_Format_Alpha(&glyph);
        glTexSubImage2D(GL_TEXTURE_2D, 0, c.x, c.y, glyph.width(), glyph.height(), GL_ALPHA, GL_UNSIGNED_BYTE, glyph.constBits());
    }
    m_textureData->pendingGlyphs.clear();
}

bool QSGDistanceFieldGlyphCache::useWorkaroundBrokenFBOReadback() const
{
    return ctx->d_ptr->workaround_brokenFBOReadBack;
}

bool QSGDistanceFieldGlyphCache::distanceFieldEnabled()
{
    return !disableDistanceField();
}

int QSGDistanceFieldGlyphCache::glyphCount() const
{
    return m_glyphCount;
}

QT_END_NAMESPACE
