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

#include "qsgdistancefieldglyphnode_p_p.h"
#include "qsgdistancefieldglyphcache_p.h"
#include <private/qsgtexture_p.h>
#include <QtOpenGL/qglfunctions.h>
#include <qmath.h>

QT_BEGIN_NAMESPACE

class QSGDistanceFieldTextMaterialShader : public QSGMaterialShader
{
public:
    QSGDistanceFieldTextMaterialShader();

    virtual void updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect);
    virtual char const *const *attributeNames() const;

protected:
    virtual void initialize();
    virtual const char *vertexShader() const;
    virtual const char *fragmentShader() const;

    void updateAlphaRange();

    qreal m_fontScale;
    qreal m_matrixScale;

    int m_matrix_id;
    int m_textureScale_id;
    int m_alphaMin_id;
    int m_alphaMax_id;
    int m_color_id;
};

const char *QSGDistanceFieldTextMaterialShader::vertexShader() const {
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

const char *QSGDistanceFieldTextMaterialShader::fragmentShader() const {
    return
        "varying highp vec2 sampleCoord;                                             \n"
        "uniform sampler2D texture;                                                  \n"
        "uniform lowp vec4 color;                                                    \n"
        "uniform highp float alphaMin;                                               \n"
        "uniform highp float alphaMax;                                               \n"
        "void main() {                                                               \n"
        "    gl_FragColor = color * smoothstep(alphaMin,                             \n"
        "                                      alphaMax,                             \n"
        "                                      texture2D(texture, sampleCoord).a);   \n"
        "}";
}

char const *const *QSGDistanceFieldTextMaterialShader::attributeNames() const {
    static char const *const attr[] = { "vCoord", "tCoord", 0 };
    return attr;
}

QSGDistanceFieldTextMaterialShader::QSGDistanceFieldTextMaterialShader()
    : m_fontScale(1.0)
    , m_matrixScale(1.0)
{
}

void QSGDistanceFieldTextMaterialShader::updateAlphaRange()
{
    qreal combinedScale = m_fontScale * m_matrixScale;
    qreal alphaMin = qMax(0.0, 0.5 - 0.07 / combinedScale);
    qreal alphaMax = qMin(0.5 + 0.07 / combinedScale, 1.0);
    m_program.setUniformValue(m_alphaMin_id, GLfloat(alphaMin));
    m_program.setUniformValue(m_alphaMax_id, GLfloat(alphaMax));
}

void QSGDistanceFieldTextMaterialShader::initialize()
{
    QSGMaterialShader::initialize();
    m_matrix_id = m_program.uniformLocation("matrix");
    m_textureScale_id = m_program.uniformLocation("textureScale");
    m_color_id = m_program.uniformLocation("color");
    m_alphaMin_id = m_program.uniformLocation("alphaMin");
    m_alphaMax_id = m_program.uniformLocation("alphaMax");
}

void QSGDistanceFieldTextMaterialShader::updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect)
{
    Q_ASSERT(oldEffect == 0 || newEffect->type() == oldEffect->type());
    QSGDistanceFieldTextMaterial *material = static_cast<QSGDistanceFieldTextMaterial *>(newEffect);
    QSGDistanceFieldTextMaterial *oldMaterial = static_cast<QSGDistanceFieldTextMaterial *>(oldEffect);

    if (oldMaterial == 0
           || material->color() != oldMaterial->color()
           || state.isOpacityDirty()) {
        QVector4D color(material->color().redF(), material->color().greenF(),
                        material->color().blueF(), material->color().alphaF());
        color *= state.opacity();
        m_program.setUniformValue(m_color_id, color);
    }

    bool updateRange = false;
    if (oldMaterial == 0
            || material->glyphCache()->fontScale() != oldMaterial->glyphCache()->fontScale()) {
        m_fontScale = material->glyphCache()->fontScale();
        updateRange = true;
    }
    if (state.isMatrixDirty()) {
        m_program.setUniformValue(m_matrix_id, state.combinedMatrix());
        m_matrixScale = qSqrt(state.modelViewMatrix().determinant());
        updateRange = true;
    }
    if (updateRange)
        updateAlphaRange();

    bool updated = material->updateTexture();
    Q_ASSERT(!material->texture().isNull());

    Q_ASSERT(oldMaterial == 0 || !oldMaterial->texture().isNull());
    if (updated
            || oldMaterial == 0
            || oldMaterial->texture()->textureId() != material->texture()->textureId()) {
        m_program.setUniformValue(m_textureScale_id, QVector2D(1.0 / material->glyphCache()->textureSize().width(),
                                                               1.0 / material->glyphCache()->textureSize().height()));
        glBindTexture(GL_TEXTURE_2D, material->texture()->textureId());

        if (updated) {
            // Set the mag/min filters to be linear. We only need to do this when the texture
            // has been recreated.
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
    }
}

QSGDistanceFieldTextMaterial::QSGDistanceFieldTextMaterial()
    : m_texture(0), m_glyph_cache(0)
{
   setFlag(Blending, true);
}

QSGDistanceFieldTextMaterial::~QSGDistanceFieldTextMaterial()
{
}

QSGMaterialType *QSGDistanceFieldTextMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader *QSGDistanceFieldTextMaterial::createShader() const
{
    return new QSGDistanceFieldTextMaterialShader;
}

bool QSGDistanceFieldTextMaterial::updateTexture()
{
    QSize glyphCacheSize = m_glyph_cache->textureSize();
    if (glyphCacheSize != m_size) {
        QSGPlainTexture *t = new QSGPlainTexture;
        t->setTextureId(m_glyph_cache->texture());
        t->setTextureSize(glyphCacheSize);
        t->setOwnsTexture(false);
        m_texture = QSGTextureRef(t);

        m_size = glyphCacheSize;

        return true;
    } else {
        return false;
    }
}

int QSGDistanceFieldTextMaterial::compare(const QSGMaterial *o) const
{
    Q_ASSERT(o && type() == o->type());
    const QSGDistanceFieldTextMaterial *other = static_cast<const QSGDistanceFieldTextMaterial *>(o);
    if (m_glyph_cache->fontScale() != other->m_glyph_cache->fontScale()) {
        qreal s1 = m_glyph_cache->fontScale();
        qreal s2 = other->m_glyph_cache->fontScale();
        return int(s2 < s1) - int(s1 < s2);
    }
    QRgb c1 = m_color.rgba();
    QRgb c2 = other->m_color.rgba();
    return int(c2 < c1) - int(c1 < c2);
}


class DistanceFieldStyledTextMaterialShader : public QSGDistanceFieldTextMaterialShader
{
public:
    DistanceFieldStyledTextMaterialShader();

    virtual void updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect);

protected:
    virtual void initialize();
    virtual const char *fragmentShader() const = 0;

    int m_styleColor_id;
};

DistanceFieldStyledTextMaterialShader::DistanceFieldStyledTextMaterialShader()
    : QSGDistanceFieldTextMaterialShader()
{
}

void DistanceFieldStyledTextMaterialShader::initialize()
{
    QSGDistanceFieldTextMaterialShader::initialize();
    m_styleColor_id = m_program.uniformLocation("styleColor");
}

void DistanceFieldStyledTextMaterialShader::updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect)
{
    QSGDistanceFieldTextMaterialShader::updateState(state, newEffect, oldEffect);

    QSGDistanceFieldStyledTextMaterial *material = static_cast<QSGDistanceFieldStyledTextMaterial *>(newEffect);
    QSGDistanceFieldStyledTextMaterial *oldMaterial = static_cast<QSGDistanceFieldStyledTextMaterial *>(oldEffect);

    if (oldMaterial == 0
           || material->styleColor() != oldMaterial->styleColor()
           || (state.isOpacityDirty())) {
        QVector4D color(material->styleColor().redF(), material->styleColor().greenF(),
                        material->styleColor().blueF(), material->styleColor().alphaF());
        color *= state.opacity();
        m_program.setUniformValue(m_styleColor_id, color);
    }
}

QSGDistanceFieldStyledTextMaterial::QSGDistanceFieldStyledTextMaterial()
    : QSGDistanceFieldTextMaterial()
{
}

QSGDistanceFieldStyledTextMaterial::~QSGDistanceFieldStyledTextMaterial()
{
}

int QSGDistanceFieldStyledTextMaterial::compare(const QSGMaterial *o) const
{
    Q_ASSERT(o && type() == o->type());
    const QSGDistanceFieldStyledTextMaterial *other = static_cast<const QSGDistanceFieldStyledTextMaterial *>(o);
    if (m_styleColor != other->m_styleColor) {
        QRgb c1 = m_styleColor.rgba();
        QRgb c2 = other->m_styleColor.rgba();
        return int(c2 < c1) - int(c1 < c2);
    }
    return QSGDistanceFieldTextMaterial::compare(o);
}


class DistanceFieldOutlineTextMaterialShader : public DistanceFieldStyledTextMaterialShader
{
public:
    DistanceFieldOutlineTextMaterialShader();

    virtual void updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect);

protected:
    virtual void initialize();
    virtual const char *fragmentShader() const;

    void updateOutlineAlphaRange();

    int m_outlineAlphaMax0_id;
    int m_outlineAlphaMax1_id;
};

const char *DistanceFieldOutlineTextMaterialShader::fragmentShader() const {
    return
            "varying highp vec2 sampleCoord;                                                      \n"
            "uniform sampler2D texture;                                                           \n"
            "uniform lowp vec4 color;                                                             \n"
            "uniform lowp vec4 styleColor;                                                        \n"
            "uniform highp float alphaMin;                                                        \n"
            "uniform highp float alphaMax;                                                        \n"
            "uniform highp float outlineAlphaMax0;                                                \n"
            "uniform highp float outlineAlphaMax1;                                                \n"
            "void main() {                                                                        \n"
            "    mediump float d = texture2D(texture, sampleCoord).a;                             \n"
            "    gl_FragColor = mix(styleColor, color, smoothstep(alphaMin, alphaMax, d))         \n"
            "                       * smoothstep(outlineAlphaMax0, outlineAlphaMax1, d);          \n"
            "}";
}

DistanceFieldOutlineTextMaterialShader::DistanceFieldOutlineTextMaterialShader()
    : DistanceFieldStyledTextMaterialShader()
{
}

void DistanceFieldOutlineTextMaterialShader::initialize()
{
    DistanceFieldStyledTextMaterialShader::initialize();
    m_outlineAlphaMax0_id = m_program.uniformLocation("outlineAlphaMax0");
    m_outlineAlphaMax1_id = m_program.uniformLocation("outlineAlphaMax1");
}

void DistanceFieldOutlineTextMaterialShader::updateOutlineAlphaRange()
{
    qreal outlineLimit = qMax(qreal(0.1), qreal(0.5 - 0.1 / m_fontScale));

    qreal combinedScale = m_fontScale * m_matrixScale;
    qreal alphaMin = qMax(0.0, 0.5 - 0.07 / combinedScale);
    qreal styleAlphaMin0 = qMax(0.0, outlineLimit - 0.07 / combinedScale);
    qreal styleAlphaMin1 = qMin(qreal(outlineLimit + 0.07 / combinedScale), alphaMin);
    m_program.setUniformValue(m_outlineAlphaMax0_id, GLfloat(styleAlphaMin0));
    m_program.setUniformValue(m_outlineAlphaMax1_id, GLfloat(styleAlphaMin1));
}

void DistanceFieldOutlineTextMaterialShader::updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect)
{
    DistanceFieldStyledTextMaterialShader::updateState(state, newEffect, oldEffect);

    QSGDistanceFieldOutlineTextMaterial *material = static_cast<QSGDistanceFieldOutlineTextMaterial *>(newEffect);
    QSGDistanceFieldOutlineTextMaterial *oldMaterial = static_cast<QSGDistanceFieldOutlineTextMaterial *>(oldEffect);

    if (oldMaterial == 0
            || material->glyphCache()->fontScale() != oldMaterial->glyphCache()->fontScale()
            || state.isMatrixDirty())
        updateOutlineAlphaRange();
}


QSGDistanceFieldOutlineTextMaterial::QSGDistanceFieldOutlineTextMaterial()
    : QSGDistanceFieldStyledTextMaterial()
{
}

QSGDistanceFieldOutlineTextMaterial::~QSGDistanceFieldOutlineTextMaterial()
{
}

QSGMaterialType *QSGDistanceFieldOutlineTextMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader *QSGDistanceFieldOutlineTextMaterial::createShader() const
{
    return new DistanceFieldOutlineTextMaterialShader;
}


class DistanceFieldShiftedStyleTextMaterialShader : public DistanceFieldStyledTextMaterialShader
{
public:
    DistanceFieldShiftedStyleTextMaterialShader();

    virtual void updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect);

protected:
    virtual void initialize();
    virtual const char *fragmentShader() const;

    void updateShift(const QSGDistanceFieldGlyphCache *cache, const QPointF& shift);

    int m_shift_id;
};

DistanceFieldShiftedStyleTextMaterialShader::DistanceFieldShiftedStyleTextMaterialShader()
    : DistanceFieldStyledTextMaterialShader()
{
}

void DistanceFieldShiftedStyleTextMaterialShader::initialize()
{
    DistanceFieldStyledTextMaterialShader::initialize();
    m_shift_id = m_program.uniformLocation("shift");
}

void DistanceFieldShiftedStyleTextMaterialShader::updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect)
{
    DistanceFieldStyledTextMaterialShader::updateState(state, newEffect, oldEffect);

    QSGDistanceFieldShiftedStyleTextMaterial *material = static_cast<QSGDistanceFieldShiftedStyleTextMaterial *>(newEffect);
    QSGDistanceFieldShiftedStyleTextMaterial *oldMaterial = static_cast<QSGDistanceFieldShiftedStyleTextMaterial *>(oldEffect);

    if (oldMaterial == 0
            || oldMaterial->glyphCache()->fontScale() != material->glyphCache()->fontScale()
            || oldMaterial->shift() != material->shift()
            || oldMaterial->texture()->textureSize() != material->texture()->textureSize()) {
        updateShift(material->glyphCache(), material->shift());
    }
}

void DistanceFieldShiftedStyleTextMaterialShader::updateShift(const QSGDistanceFieldGlyphCache *cache, const QPointF &shift)
{
    QPointF texel(1.0 / cache->fontScale() / cache->textureSize().width() * shift.x(),
                  1.0 / cache->fontScale() / cache->textureSize().height() * shift.y());
    m_program.setUniformValue(m_shift_id, texel);
}

const char *DistanceFieldShiftedStyleTextMaterialShader::fragmentShader() const {
    return
            "varying highp vec2 sampleCoord;                                                       \n"
            "uniform sampler2D texture;                                                            \n"
            "uniform lowp vec4 color;                                                              \n"
            "uniform lowp vec4 styleColor;                                                         \n"
            "uniform highp float alphaMin;                                                         \n"
            "uniform highp float alphaMax;                                                         \n"
            "uniform highp vec2 shift;                                                             \n"
            "void main() {                                                                         \n"
            "    highp float a = smoothstep(alphaMin, alphaMax, texture2D(texture, sampleCoord).a);\n"
            "    highp vec4 shifted = styleColor * smoothstep(alphaMin,                            \n"
            "                                                 alphaMax,                            \n"
            "                                                 texture2D(texture, sampleCoord - shift).a); \n"
            "    gl_FragColor = mix(shifted, color, a);                                            \n"
            "}";
}

QSGDistanceFieldShiftedStyleTextMaterial::QSGDistanceFieldShiftedStyleTextMaterial()
    : QSGDistanceFieldStyledTextMaterial()
{
}

QSGDistanceFieldShiftedStyleTextMaterial::~QSGDistanceFieldShiftedStyleTextMaterial()
{
}

QSGMaterialType *QSGDistanceFieldShiftedStyleTextMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader *QSGDistanceFieldShiftedStyleTextMaterial::createShader() const
{
    return new DistanceFieldShiftedStyleTextMaterialShader;
}

QT_END_NAMESPACE
