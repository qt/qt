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

#include "default_texturenode.h"
#include "qsgtextureprovider.h"

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


class TextureProviderMaterialShader : public AbstractMaterialShader
{
public:
    virtual void updateState(const RenderState &state, AbstractMaterial *newEffect, AbstractMaterial *oldEffect);
    virtual char const *const *attributeNames() const;

    static AbstractMaterialType type;

protected:
    virtual void initialize();
    virtual const char *vertexShader() const;
    virtual const char *fragmentShader() const;

    int m_matrix_id;
};


const char *TextureProviderMaterialShader::vertexShader() const
{
    return qt_material_vertex_code;
}

const char *TextureProviderMaterialShader::fragmentShader() const
{
    return qt_material_fragment_code;
}

AbstractMaterialType TextureProviderMaterialShader::type;

char const *const *TextureProviderMaterialShader::attributeNames() const
{
    static char const *const attr[] = { "qt_Vertex", "qt_MultiTexCoord0", 0 };
    return attr;
}

void TextureProviderMaterialShader::initialize()
{
    m_matrix_id = m_program.uniformLocation("qt_Matrix");
    if (m_program.isLinked()) {
        m_program.bind();
        m_program.setUniformValue("qt_Texture", GLuint(0));
    }
}

void TextureProviderMaterialShader::updateState(const RenderState &state, AbstractMaterial *newEffect, AbstractMaterial *oldEffect)
{
    Q_ASSERT(oldEffect == 0 || newEffect->type() == oldEffect->type());
    QSGTextureProvider *tx = static_cast<TextureProviderMaterial *>(newEffect)->texture();
    QSGTextureProvider *oldTx = oldEffect ? static_cast<TextureProviderMaterial *>(oldEffect)->texture() : 0;

    tx->bind(oldTx ? oldTx->texture().texture() : 0);

    if (state.isMatrixDirty())
        m_program.setUniformValue(m_matrix_id, state.combinedMatrix());
}


void TextureProviderMaterial::setTexture(QSGTextureProvider *texture)
{
    m_texture = texture;
    bool opaque = m_texture ? m_texture->opaque() : true;
    setFlag(Blending, !opaque);
}

AbstractMaterialType *TextureProviderMaterial::type() const
{
    return &TextureProviderMaterialShader::type;
}

AbstractMaterialShader *TextureProviderMaterial::createShader() const
{
    return new TextureProviderMaterialShader;
}

int TextureProviderMaterial::compare(const AbstractMaterial *o) const
{
    Q_ASSERT(o && type() == o->type());
    const TextureProviderMaterial *other = static_cast<const TextureProviderMaterial *>(o);
    if (int diff = m_texture->texture().texture() - other->texture()->texture().texture())
        return diff;
    if (int diff = int(m_texture->opaque()) - int(other->m_texture->opaque()))
        return diff;
    if (int diff = int(m_texture->horizontalWrapMode()) - int(other->m_texture->horizontalWrapMode()))
        return diff;
    if (int diff = int(m_texture->verticalWrapMode()) - int(other->m_texture->verticalWrapMode()))
        return diff;
    if (int diff = int(m_texture->filtering()) - int(other->m_texture->filtering()))
        return diff;
    return int(m_texture->mipmap()) - int(other->m_texture->mipmap());
}

bool TextureProviderMaterial::is(const AbstractMaterial *effect)
{
    return effect->type() == &TextureProviderMaterialShader::type;
}


class TextureProviderMaterialWithOpacityShader : public TextureProviderMaterialShader
{
public:
    virtual void updateState(const RenderState &state, AbstractMaterial *newEffect, AbstractMaterial *oldEffect);
    virtual void initialize();

    static AbstractMaterialType type;

protected:
    virtual const char *fragmentShader() const { return qt_material_opacity_fragment_code; }

    int m_opacity_id;
};

AbstractMaterialType TextureProviderMaterialWithOpacityShader::type;

bool TextureProviderMaterialWithOpacity::is(const AbstractMaterial *effect)
{
    return effect->type() == &TextureProviderMaterialWithOpacityShader::type;
}

AbstractMaterialType *TextureProviderMaterialWithOpacity::type() const
{
    return &TextureProviderMaterialWithOpacityShader::type;
}

void TextureProviderMaterialWithOpacity::setTexture(QSGTextureProvider *texture)
{
    m_texture = texture;
    setFlag(Blending, true);
}

AbstractMaterialShader *TextureProviderMaterialWithOpacity::createShader() const
{
    return new TextureProviderMaterialWithOpacityShader;
}

void TextureProviderMaterialWithOpacityShader::updateState(const RenderState &state, AbstractMaterial *newEffect, AbstractMaterial *oldEffect)
{
    Q_ASSERT(oldEffect == 0 || newEffect->type() == oldEffect->type());

    if (state.isOpacityDirty())
        m_program.setUniformValue(m_opacity_id, state.opacity());

    TextureProviderMaterialShader::updateState(state, newEffect, oldEffect);
}

void TextureProviderMaterialWithOpacityShader::initialize()
{
    TextureProviderMaterialShader::initialize();
    m_opacity_id = m_program.uniformLocation("opacity");
}


DefaultTextureNode::DefaultTextureNode()
    : m_dirtyGeometry(false)
    , m_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4)
{
    setMaterial(&m_materialO);
    setOpaqueMaterial(&m_material);
    setGeometry(&m_geometry);

#ifdef QML_RUNTIME_TESTING
    description = QLatin1String("pixmap");
#endif
}

void DefaultTextureNode::setTargetRect(const QRectF &rect)
{
    if (rect == m_targetRect)
        return;
    m_targetRect = rect;
    m_dirtyGeometry = true;
    markDirty(DirtyGeometry);
}

void DefaultTextureNode::setSourceRect(const QRectF &rect)
{
    if (rect == m_sourceRect)
        return;
    m_sourceRect = rect;
    m_dirtyGeometry = true;
    markDirty(DirtyGeometry);
}

void DefaultTextureNode::setTexture(QSGTextureProvider *texture)
{
    if (texture == m_texture)
        return;

    m_texture = texture;
    m_material.setTexture(texture);
    m_materialO.setTexture(texture);
    markDirty(DirtyMaterial);

    // Because the texture can be a different part of the atlas, we need to update it...
    m_dirtyGeometry = true;
}

void DefaultTextureNode::update()
{
    if (m_dirtyGeometry)
        updateGeometry();
}

void DefaultTextureNode::preprocess()
{
    m_texture->updateTexture();
    updateGeometry();
}

void DefaultTextureNode::updateGeometry()
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
