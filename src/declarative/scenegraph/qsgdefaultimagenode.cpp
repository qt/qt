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

#include "qsgdefaultimagenode_p.h"

#include <private/qsgtextureprovider_p.h>

QT_BEGIN_NAMESPACE

static const char qt_material_vertex_code[] =
    "uniform highp mat4 qt_Matrix;                      \n"
    "attribute highp vec4 qt_Vertex;                    \n"
    "attribute highp vec2 qt_MultiTexCoord0;            \n"
    "varying highp vec2 qt_TexCoord0;                   \n"
    "void main() {                                      \n"
    "    qt_TexCoord0 = qt_MultiTexCoord0;              \n"
    "    gl_Position = qt_Matrix * qt_Vertex;           \n"
    "}";

static const char qt_material_fragment_code[] =
    "varying highp vec2 qt_TexCoord0;                   \n"
    "uniform sampler2D qt_Texture;                      \n"
    "void main() {                                      \n"
    "    gl_FragColor = texture2D(qt_Texture, qt_TexCoord0); \n"
    "}";

static const char qt_material_opacity_fragment_code[] =
    "varying highp vec2 qt_TexCoord0;                   \n"
    "uniform sampler2D qt_Texture;                      \n"
    "uniform lowp float opacity;                        \n"
    "void main() {                                      \n"
    "    gl_FragColor = texture2D(qt_Texture, qt_TexCoord0) * opacity; \n"
    "}";


class QSGTextureProviderMaterialShader : public QSGMaterialShader
{
public:
    virtual void updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect);
    virtual char const *const *attributeNames() const;

    static QSGMaterialType type;

protected:
    virtual void initialize();
    virtual const char *vertexShader() const;
    virtual const char *fragmentShader() const;

    int m_matrix_id;
};


const char *QSGTextureProviderMaterialShader::vertexShader() const
{
    return qt_material_vertex_code;
}

const char *QSGTextureProviderMaterialShader::fragmentShader() const
{
    return qt_material_fragment_code;
}

QSGMaterialType QSGTextureProviderMaterialShader::type;

char const *const *QSGTextureProviderMaterialShader::attributeNames() const
{
    static char const *const attr[] = { "qt_Vertex", "qt_MultiTexCoord0", 0 };
    return attr;
}

void QSGTextureProviderMaterialShader::initialize()
{
    m_matrix_id = m_program.uniformLocation("qt_Matrix");
    if (m_program.isLinked()) {
        m_program.bind();
        m_program.setUniformValue("qt_Texture", GLuint(0));
    }
}

void QSGTextureProviderMaterialShader::updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect)
{
    Q_ASSERT(oldEffect == 0 || newEffect->type() == oldEffect->type());
    QSGTextureProvider *tx = static_cast<QSGTextureProviderMaterial *>(newEffect)->texture();
    QSGTextureProvider *oldTx = oldEffect ? static_cast<QSGTextureProviderMaterial *>(oldEffect)->texture() : 0;

    tx->bind(oldTx ? oldTx->texture().texture() : 0);

    if (state.isMatrixDirty())
        m_program.setUniformValue(m_matrix_id, state.combinedMatrix());
}


void QSGTextureProviderMaterial::setTexture(QSGTextureProvider *texture)
{
    m_texture = texture;
}

void QSGTextureProviderMaterial::setBlending(bool enable)
{
    setFlag(Blending, enable);
}

bool QSGTextureProviderMaterial::blending() const
{
    return flags() & Blending;
}

QSGMaterialType *QSGTextureProviderMaterial::type() const
{
    return &QSGTextureProviderMaterialShader::type;
}

QSGMaterialShader *QSGTextureProviderMaterial::createShader() const
{
    return new QSGTextureProviderMaterialShader;
}

int QSGTextureProviderMaterial::compare(const QSGMaterial *o) const
{
    Q_ASSERT(o && type() == o->type());
    const QSGTextureProviderMaterial *other = static_cast<const QSGTextureProviderMaterial *>(o);
    if (int diff = m_texture->texture().texture() - other->texture()->texture().texture())
        return diff;
    if (int diff = int(m_texture->horizontalWrapMode()) - int(other->m_texture->horizontalWrapMode()))
        return diff;
    if (int diff = int(m_texture->verticalWrapMode()) - int(other->m_texture->verticalWrapMode()))
        return diff;
    if (int diff = int(m_texture->filtering()) - int(other->m_texture->filtering()))
        return diff;
    return int(m_texture->mipmapFiltering()) - int(other->m_texture->mipmapFiltering());
}

bool QSGTextureProviderMaterial::is(const QSGMaterial *effect)
{
    return effect->type() == &QSGTextureProviderMaterialShader::type;
}


class TextureProviderMaterialWithOpacityShader : public QSGTextureProviderMaterialShader
{
public:
    virtual void updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect);
    virtual void initialize();

    static QSGMaterialType type;

protected:
    virtual const char *fragmentShader() const { return qt_material_opacity_fragment_code; }

    int m_opacity_id;
};

QSGMaterialType TextureProviderMaterialWithOpacityShader::type;

bool QSGTextureProviderMaterialWithOpacity::is(const QSGMaterial *effect)
{
    return effect->type() == &TextureProviderMaterialWithOpacityShader::type;
}

QSGMaterialType *QSGTextureProviderMaterialWithOpacity::type() const
{
    return &TextureProviderMaterialWithOpacityShader::type;
}

void QSGTextureProviderMaterialWithOpacity::setTexture(QSGTextureProvider *texture)
{
    m_texture = texture;
    setFlag(Blending, true);
}

QSGMaterialShader *QSGTextureProviderMaterialWithOpacity::createShader() const
{
    return new TextureProviderMaterialWithOpacityShader;
}

void TextureProviderMaterialWithOpacityShader::updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect)
{
    Q_ASSERT(oldEffect == 0 || newEffect->type() == oldEffect->type());

    if (state.isOpacityDirty())
        m_program.setUniformValue(m_opacity_id, state.opacity());

    QSGTextureProviderMaterialShader::updateState(state, newEffect, oldEffect);
}

void TextureProviderMaterialWithOpacityShader::initialize()
{
    QSGTextureProviderMaterialShader::initialize();
    m_opacity_id = m_program.uniformLocation("opacity");
}


QSGDefaultImageNode::QSGDefaultImageNode()
    : m_texture(0)
    , m_sourceRect(0, 1, 1, -1)
    , m_dirtyGeometry(false)
    , m_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4)
{
    setMaterial(&m_materialO);
    setOpaqueMaterial(&m_material);
    setGeometry(&m_geometry);

#ifdef QML_RUNTIME_TESTING
    description = QLatin1String("pixmap");
#endif
}

void QSGDefaultImageNode::setTargetRect(const QRectF &rect)
{
    if (rect == m_targetRect)
        return;
    m_targetRect = rect;
    m_dirtyGeometry = true;
    markDirty(DirtyGeometry);
}

void QSGDefaultImageNode::setSourceRect(const QRectF &rect)
{
    if (rect == m_sourceRect)
        return;
    m_sourceRect = rect;
    m_dirtyGeometry = true;
    markDirty(DirtyGeometry);
}

void QSGDefaultImageNode::setTexture(QSGTextureProvider *texture)
{
    if (texture == m_texture)
        return;

    m_texture = texture;
    m_material.setTexture(texture);
    m_materialO.setTexture(texture);
    if (texture && !texture->texture().isNull())
        m_material.setBlending(texture->texture()->hasAlphaChannel());
    markDirty(DirtyMaterial);

    // Because the texture can be a different part of the atlas, we need to update it...
    m_dirtyGeometry = true;
}

void QSGDefaultImageNode::update()
{
    if (m_dirtyGeometry)
        updateGeometry();
}

void QSGDefaultImageNode::preprocess()
{
    m_texture->updateTexture();
    bool alpha = m_material.blending();
    if (!m_texture->texture().isNull() && alpha != m_texture->texture()->hasAlphaChannel()) {
        m_material.setBlending(!alpha);
        markDirty(DirtyMaterial);
    }
    updateGeometry();
}

void QSGDefaultImageNode::updateGeometry()
{
    const QSGTextureRef t = m_texture->texture();
    if (t.isNull()) {
        QSGGeometry::updateTexturedRectGeometry(&m_geometry, QRectF(), QRectF());
    } else {
        QRectF textureRect = t->textureSubRect();
        QRectF sr(textureRect.x() + m_sourceRect.x() * textureRect.width(),
                  textureRect.y() + m_sourceRect.y() * textureRect.height(),
                  m_sourceRect.width() * textureRect.width(),
                  m_sourceRect.height() * textureRect.height());

        QSGGeometry::updateTexturedRectGeometry(&m_geometry, m_targetRect, sr);
    }
    markDirty(DirtyGeometry);
    m_dirtyGeometry = false;
}

QT_END_NAMESPACE
