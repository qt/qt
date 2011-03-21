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

#include <qmath.h>
#include <private/qtriangulator_p.h>
#include <private/qdeclarativeglobal_p.h>
#include <private/qsgtexture_p.h>

QT_BEGIN_NAMESPACE

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

    //   v1
    //  /  \
    // v4  v2
    //  \  /
    //   v3

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

static QImage makeDistanceField(const QPainterPath &path, float offs)
{
    QImage image(QT_DISTANCEFIELD_TILESIZE, QT_DISTANCEFIELD_TILESIZE, QImage::Format_ARGB32_Premultiplied);

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
    p.scale(1 / qreal(QT_DISTANCEFIELD_SCALE), 1 / qreal(QT_DISTANCEFIELD_SCALE));
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
            v.p.x = float(to[0] / QT_DISTANCEFIELD_SCALE) + offs - 0.5f;
            v.p.y = float(to[1] / QT_DISTANCEFIELD_SCALE) + offs - 0.5f;
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
            iLine[x] = QRgb(fLine[x] + 127) << 24;
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

DEFINE_BOOL_CONFIG_OPTION(enableDistanceField, QML_ENABLE_DISTANCEFIELD)

QHash<QString, DistanceFieldFontAtlas *> DistanceFieldFontAtlas::m_atlases;
QHash<QString, DistanceFieldFontAtlas::DistanceFieldTextureData *> DistanceFieldFontAtlas::m_textures_data;

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

DistanceFieldFontAtlas::DistanceFieldTextureData *DistanceFieldFontAtlas::textureData()
{
    QHash<QString, DistanceFieldTextureData *>::iterator data = m_textures_data.find(m_distanceFieldKey);
    if (data == m_textures_data.end())
        data = m_textures_data.insert(m_distanceFieldKey, new DistanceFieldTextureData);
    return data.value();
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
    m_distanceFieldKey = basename + bold + italic;
    m_textureData = textureData();

    m_glyphCount = m_fontEngine->glyphCount();
}

qreal DistanceFieldFontAtlas::glyphMargin() const
{
    return QT_DISTANCEFIELD_MARGIN / qreal(QT_DISTANCEFIELD_SCALE);
}

QSGTextureRef DistanceFieldFontAtlas::texture()
{
    if (m_textureData->texture.isNull())
        m_textureData->texture = createTexture();

    return m_textureData->texture;
}

void DistanceFieldFontAtlas::populate(int count, const glyph_t *glyphs)
{
    glBindTexture(GL_TEXTURE_2D, texture()->textureId());

    for (int i = 0; i < count; ++i) {
        if (m_textureData->generatedGlyphs.contains(glyphs[i]))
            continue;

        QImage glyph = renderDistanceFieldGlyph(glyphs[i]);
        convert_to_Format_Alpha(&glyph);

        int x = (glyphs[i] * QT_DISTANCEFIELD_TILESIZE) % atlasSize().width();
        int y = ((glyphs[i] * QT_DISTANCEFIELD_TILESIZE) / atlasSize().width()) * QT_DISTANCEFIELD_TILESIZE;

        glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, glyph.width(), glyph.height(), GL_ALPHA, GL_UNSIGNED_BYTE, glyph.constBits());

        m_textureData->generatedGlyphs.insert(glyphs[i]);
    }
}

QSize DistanceFieldFontAtlas::atlasSize() const
{
    if (!m_textureData->size.isValid()) {
        const int texWidth = QT_DISTANCEFIELD_TEXTURESIZE;
        const int texHeight = ((glyphCount() * QT_DISTANCEFIELD_TILESIZE) / texWidth + 1) * QT_DISTANCEFIELD_TILESIZE;
        m_textureData->size = QSize(texWidth, texHeight);
    }
    return m_textureData->size;
}

DistanceFieldFontAtlas::Metrics DistanceFieldFontAtlas::glyphMetrics(glyph_t glyph)
{
    QHash<glyph_t, Metrics>::iterator metric = m_metrics.find(glyph);
    if (metric == m_metrics.end()) {
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

    return metric.value();
}

DistanceFieldFontAtlas::TexCoord DistanceFieldFontAtlas::glyphTexCoord(glyph_t glyph)
{
    if (glyph >= glyphCount())
        qWarning("Warning: distance-field glyph is not available with index %d", glyph);

    QHash<glyph_t, TexCoord>::iterator texCoord = m_textureData->texCoords.find(glyph);
    if (texCoord == m_textureData->texCoords.end()) {
        const QSize texSize = atlasSize();

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

        texCoord = m_textureData->texCoords.insert(glyph, c);
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

QSGTextureRef DistanceFieldFontAtlas::createTexture()
{
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    const QSize size = atlasSize();
    QVarLengthArray<uchar> data(size.width() * size.height());
    for (int i = 0; i < data.size(); ++i)
        data[i] = 0;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, size.width(), size.height(), 0, GL_ALPHA, GL_UNSIGNED_BYTE, &data[0]);

    GLuint error = glGetError();
    if (error != GL_NO_ERROR) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &id);
        return QSGTextureRef();
    }

    QSGPlainTexture *texture = new QSGPlainTexture;
    texture->setTextureId(id);
    texture->setTextureSize(size);
    texture->setHasAlphaChannel(true);

    return QSGTextureRef(texture);
}

bool DistanceFieldFontAtlas::distanceFieldEnabled()
{
    return enableDistanceField();
}

int DistanceFieldFontAtlas::glyphCount() const
{
    return m_glyphCount;
}

QPointF DistanceFieldFontAtlas::pixelToTexel(const QPointF &pixel) const
{
    const QSize texSize = atlasSize();
    qreal glyphTexel = 1.0 / qreal(QT_DISTANCEFIELD_TILESIZE) / scaleRatioFromRefSize();
    QPointF texel(glyphTexel * (qreal(QT_DISTANCEFIELD_TILESIZE) / texSize.width()),
                  glyphTexel * (qreal(QT_DISTANCEFIELD_TILESIZE) / texSize.height()));

    return QPointF(pixel.x() * texel.x(), pixel.y() * texel.y());
}

QT_END_NAMESPACE
