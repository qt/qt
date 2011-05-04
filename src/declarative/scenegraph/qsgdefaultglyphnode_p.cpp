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

#include "qsgdefaultglyphnode_p_p.h"

#include <qglshaderprogram.h>

#include <private/qtextureglyphcache_gl_p.h>
#include <private/qfontengine_p.h>
#include <private/qglextensions_p.h>

#include <private/qsgtexture_p.h>

#include <private/qrawfont_p.h>

QT_BEGIN_NAMESPACE

class QSGTextMaskMaterialData : public QSGMaterialShader
{
public:
    QSGTextMaskMaterialData();

    virtual void updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect);
    virtual char const *const *attributeNames() const;
private:
    virtual void initialize();
    virtual const char *vertexShader() const;
    virtual const char *fragmentShader() const;

    int m_matrix_id;
    int m_color_id;
    int m_textureScale_id;
};

const char *QSGTextMaskMaterialData::vertexShader() const {
    return
        "uniform highp mat4 matrix;                     \n"
        "uniform highp vec2 textureScale;               \n"
        "attribute highp vec4 vCoord;                   \n"
        "attribute highp vec2 tCoord;                   \n"
        "varying highp vec2 sampleCoord;                \n"
        "void main() {                                  \n"
        "     sampleCoord = tCoord * textureScale;      \n"
        "     gl_Position = matrix * vCoord;            \n"
        "}";
}

const char *QSGTextMaskMaterialData::fragmentShader() const {
    return
        "varying highp vec2 sampleCoord;                \n"
        "uniform sampler2D texture;                     \n"
        "uniform lowp vec4 color;                       \n"
        "void main() {                                  \n"
        "    gl_FragColor = color * texture2D(texture, sampleCoord).a; \n"
        "}";
}

char const *const *QSGTextMaskMaterialData::attributeNames() const
{
    static char const *const attr[] = { "vCoord", "tCoord", 0 };
    return attr;
}

QSGTextMaskMaterialData::QSGTextMaskMaterialData()
{
}

void QSGTextMaskMaterialData::initialize()
{
    m_matrix_id = m_program.uniformLocation("matrix");
    m_color_id = m_program.uniformLocation("color");
    m_textureScale_id = m_program.uniformLocation("textureScale");
}

void QSGTextMaskMaterialData::updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect)
{
    Q_ASSERT(oldEffect == 0 || newEffect->type() == oldEffect->type());
    QSGTextMaskMaterial *material = static_cast<QSGTextMaskMaterial *>(newEffect);
    QSGTextMaskMaterial *oldMaterial = static_cast<QSGTextMaskMaterial *>(oldEffect);

    if (oldMaterial == 0 || material->color() != oldMaterial->color() || state.isOpacityDirty()) {
        QVector4D color(material->color().redF(), material->color().greenF(),
                        material->color().blueF(), material->color().alphaF());
        color *= state.opacity();
        m_program.setUniformValue(m_color_id, color);
    }

    bool updated = material->ensureUpToDate();
    Q_ASSERT(material->texture());

    Q_ASSERT(oldMaterial == 0 || oldMaterial->texture());
    if (updated
            || oldMaterial == 0
            || oldMaterial->texture()->textureId() != material->texture()->textureId()) {
        m_program.setUniformValue(m_textureScale_id, QVector2D(1.0 / material->cacheTextureWidth(),
                                                               1.0 / material->cacheTextureHeight()));
        glBindTexture(GL_TEXTURE_2D, material->texture()->textureId());

        // Set the mag/min filters to be linear. We only need to do this when the texture
        // has been recreated.
        if (updated) {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
    }

    if (state.isMatrixDirty())
        m_program.setUniformValue(m_matrix_id, state.combinedMatrix());
}

QSGTextMaskMaterial::QSGTextMaskMaterial(const QRawFont &font)
    : m_texture(0), m_glyphCache(), m_font(font)
{
    init();
}

QSGTextMaskMaterial::~QSGTextMaskMaterial()
{
}

void QSGTextMaskMaterial::init()
{
    Q_ASSERT(m_font.isValid());

    QFontEngineGlyphCache::Type type = QFontEngineGlyphCache::Raster_A8;
    setFlag(Blending, true);

    QGLContext *ctx = const_cast<QGLContext *>(QGLContext::currentContext());
    Q_ASSERT(ctx != 0);

    QRawFontPrivate *fontD = QRawFontPrivate::get(m_font);
    if (fontD->fontEngine != 0) {
        m_glyphCache = fontD->fontEngine->glyphCache(ctx, type, QTransform());
        if (!m_glyphCache || m_glyphCache->cacheType() != type) {
            m_glyphCache = new QGLTextureGlyphCache(ctx, type, QTransform());
            fontD->fontEngine->setGlyphCache(ctx, m_glyphCache.data());
        }
    }

#if !defined(QT_OPENGL_ES_2)
    bool success = qt_resolve_version_2_0_functions(ctx)
                   && qt_resolve_buffer_extensions(ctx);
    Q_ASSERT(success);
    Q_UNUSED(success);
#endif
}

void QSGTextMaskMaterial::populate(const QPointF &p,
                                const QVector<quint32> &glyphIndexes,
                                const QVector<QPointF> &glyphPositions,
                                QSGGeometry *geometry,
                                QRectF *boundingRect,
                                QPointF *baseLine)
{
    Q_ASSERT(m_font.isValid());
    QVector<QFixedPoint> fixedPointPositions;
    for (int i=0; i<glyphPositions.size(); ++i)
        fixedPointPositions.append(QFixedPoint::fromPointF(glyphPositions.at(i)));

    QTextureGlyphCache *cache = glyphCache();

    QRawFontPrivate *fontD = QRawFontPrivate::get(m_font);
    cache->populate(fontD->fontEngine, glyphIndexes.size(), glyphIndexes.constData(),
                    fixedPointPositions.data());
    cache->fillInPendingGlyphs();

    int margin = cache->glyphMargin();

    Q_ASSERT(geometry->indexType() == GL_UNSIGNED_SHORT);
    geometry->allocate(glyphIndexes.size() * 4, glyphIndexes.size() * 6);
    QVector4D *vp = (QVector4D *)geometry->vertexDataAsTexturedPoint2D();
    Q_ASSERT(geometry->stride() == sizeof(QVector4D));
    ushort *ip = geometry->indexDataAsUShort();

    QPointF position(p.x(), p.y() - m_font.ascent());
    bool supportsSubPixelPositions = fontD->fontEngine->supportsSubPixelPositions();
    for (int i=0; i<glyphIndexes.size(); ++i) {
         QFixed subPixelPosition;
         if (supportsSubPixelPositions)
             subPixelPosition = cache->subPixelPositionForX(QFixed::fromReal(glyphPositions.at(i).x()));

         QTextureGlyphCache::GlyphAndSubPixelPosition glyph(glyphIndexes.at(i), subPixelPosition);
         const QTextureGlyphCache::Coord &c = cache->coords.value(glyph);

         QPointF glyphPosition = glyphPositions.at(i) + position;
         int x = qRound(glyphPosition.x()) + c.baseLineX - margin;
         int y = qRound(glyphPosition.y()) - c.baseLineY - margin;

         *boundingRect |= QRectF(x + margin, y + margin, c.w, c.h);

         float cx1 = x;
         float cx2 = x + c.w;
         float cy1 = y;
         float cy2 = y + c.h;

         float tx1 = c.x;
         float tx2 = (c.x + c.w);
         float ty1 = c.y;
         float ty2 = (c.y + c.h);

         if (baseLine->isNull())
             *baseLine = glyphPosition;

         vp[4 * i + 0] = QVector4D(cx1, cy1, tx1, ty1);
         vp[4 * i + 1] = QVector4D(cx2, cy1, tx2, ty1);
         vp[4 * i + 2] = QVector4D(cx1, cy2, tx1, ty2);
         vp[4 * i + 3] = QVector4D(cx2, cy2, tx2, ty2);

         int o = i * 4;
         ip[6 * i + 0] = o + 0;
         ip[6 * i + 1] = o + 2;
         ip[6 * i + 2] = o + 3;
         ip[6 * i + 3] = o + 3;
         ip[6 * i + 4] = o + 1;
         ip[6 * i + 5] = o + 0;
    }
}

QSGMaterialType *QSGTextMaskMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QGLTextureGlyphCache *QSGTextMaskMaterial::glyphCache() const
{
    return static_cast<QGLTextureGlyphCache*>(m_glyphCache.data());
}

QSGMaterialShader *QSGTextMaskMaterial::createShader() const
{
    return new QSGTextMaskMaterialData;
}

int QSGTextMaskMaterial::compare(const QSGMaterial *o) const
{
    Q_ASSERT(o && type() == o->type());
    const QSGTextMaskMaterial *other = static_cast<const QSGTextMaskMaterial *>(o);
    if (m_glyphCache != other->m_glyphCache)
        return m_glyphCache - other->m_glyphCache;
    QRgb c1 = m_color.rgba();
    QRgb c2 = other->m_color.rgba();
    return int(c2 < c1) - int(c1 < c2);
}

bool QSGTextMaskMaterial::ensureUpToDate()
{
    QSize glyphCacheSize(glyphCache()->width(), glyphCache()->height());
    if (glyphCacheSize != m_size) {
        if (m_texture)
            delete m_texture;
        m_texture = new QSGPlainTexture();
        m_texture->setTextureId(glyphCache()->texture());
        m_texture->setTextureSize(QSize(glyphCache()->width(), glyphCache()->height()));
        m_texture->setOwnsTexture(false);

        m_size = glyphCacheSize;

        return true;
    } else {
        return false;
    }
}

int QSGTextMaskMaterial::cacheTextureWidth() const
{
    return glyphCache()->width();
}

int QSGTextMaskMaterial::cacheTextureHeight() const
{
    return glyphCache()->height();
}

QT_END_NAMESPACE
