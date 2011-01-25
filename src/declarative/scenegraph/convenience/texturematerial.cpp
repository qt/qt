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

#include "texturematerial.h"

#include <qglshaderprogram.h>

const char qt_scenegraph_texture_material_vertex_code[] =
    "uniform highp mat4 qt_Matrix;                      \n"
    "attribute highp vec4 qt_VertexPosition;            \n"
    "attribute highp vec2 qt_VertexTexCoord;            \n"
    "varying highp vec2 qt_TexCoord;                    \n"
    "void main() {                                      \n"
    "    qt_TexCoord = qt_VertexTexCoord;               \n"
    "    gl_Position = qt_Matrix * qt_VertexPosition;   \n"
    "}";

const char qt_scenegraph_texture_material_fragment[] =
    "varying highp vec2 qt_TexCoord;                    \n"
    "uniform sampler2D qt_Texture;                      \n"
    "void main() {                                      \n"
    "    gl_FragColor = texture2D(qt_Texture, qt_TexCoord);\n"
    "}";


const char *TextureMaterialData::vertexShader() const
{
    return qt_scenegraph_texture_material_vertex_code;
}

const char *TextureMaterialData::fragmentShader() const
{
    return qt_scenegraph_texture_material_fragment;
}

const TextureMaterialData::Attributes TextureMaterialData::attributes() const
{
    Attributes attr = {
        attributeIds,
        attributeNames
    };
    return attr;
}

const QSG::VertexAttribute *TextureMaterialData::requiredFields() const
{
    return attributeIds;
}

AbstractEffectType TextureMaterialData::type;


const QSG::VertexAttribute TextureMaterialData::attributeIds[] = {
    QSG::Position,
    QSG::TextureCoord0,
    QSG::VertexAttribute(-1)
};

const char *const TextureMaterialData::attributeNames[] = {
    "qt_VertexPosition",
    "qt_VertexTexCoord",
    0
};

void TextureMaterialData::initialize()
{
    AbstractShaderEffectProgram::initialize();
    m_matrix_id = m_program.uniformLocation("qt_Matrix");
}

void TextureMaterialData::updateRendererState(Renderer *renderer, Renderer::Updates updates)
{
    if (updates & Renderer::UpdateMatrices)
        m_program.setUniformValue(m_matrix_id, renderer->combinedMatrix());
}

void TextureMaterialData::updateEffectState(Renderer *renderer, AbstractEffect *newEffect, AbstractEffect *oldEffect)
{
    Q_ASSERT(oldEffect == 0 || newEffect->type() == oldEffect->type());
    TextureMaterial *tx = static_cast<TextureMaterial *>(newEffect);
    TextureMaterial *oldTx = static_cast<TextureMaterial *>(oldEffect);

    if (oldEffect == 0 || tx->texture().texture() != oldTx->texture().texture()) {
        renderer->setTexture(0, tx->texture());
        oldEffect = 0; // Force filtering update.
    }

    if (oldEffect == 0 || tx->linearFiltering() != oldTx->linearFiltering()) {
        int filtering = tx->linearFiltering() ? GL_LINEAR : GL_NEAREST;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
    }

    if (oldEffect == 0 || tx->clampToEdge() != oldTx->clampToEdge()) {
        int wrapMode = tx->clampToEdge() ? GL_CLAMP_TO_EDGE : GL_REPEAT;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    }
}


void TextureMaterial::setTexture(const QSGTextureRef &texture, bool opaque)
{
    m_texture = texture;
    m_opaque = opaque;
    setFlags(opaque ? Flag(0) : Blending);
}

AbstractEffectType *TextureMaterial::type() const
{
    return &TextureMaterialData::type;
}

AbstractEffectProgram *TextureMaterial::createProgram() const
{
    return new TextureMaterialData;
}

int TextureMaterial::compare(const AbstractEffect *o) const
{
    Q_ASSERT(o && type() == o->type());
    const TextureMaterial *other = static_cast<const TextureMaterial *>(o);
    if (int diff = m_texture->textureId() - other->texture()->textureId())
        return diff;
    return int(m_linear_filtering) - int(other->m_linear_filtering);
}

bool TextureMaterial::is(const AbstractEffect *effect)
{
    return effect->type() == &TextureMaterialData::type;
}

// TextureMaterialWithOpacity

static const char qt_scenegraph_texture_material_opacity_fragment[] =
    "varying highp vec2 qt_TexCoord;                       \n"
    "uniform sampler2D qt_Texture;                         \n"
    "uniform lowp float opacity;                        \n"
    "void main() {                                      \n"
    "    gl_FragColor = texture2D(qt_Texture, qt_TexCoord) * opacity; \n"
    "}";

class TextureMaterialWithOpacityData : public TextureMaterialData
{
public:
    virtual void updateEffectState(Renderer *renderer, AbstractEffect *newEffect, AbstractEffect *oldEffect);
    virtual void initialize();
    virtual const char *fragmentShader() const { return qt_scenegraph_texture_material_opacity_fragment; }

    static AbstractEffectType type;

    int m_opacity_id;
};
AbstractEffectType TextureMaterialWithOpacityData::type;

bool TextureMaterialWithOpacity::is(const AbstractEffect *effect)
{
    return effect->type() == &TextureMaterialWithOpacityData::type;
}

AbstractEffectType *TextureMaterialWithOpacity::type() const
{
    return &TextureMaterialWithOpacityData::type;
}

void TextureMaterialWithOpacity::setOpacity(qreal opacity)
{
    m_opacity = opacity;
    setFlags(m_opaque && opacity == 1 ? Flag(0) : Blending);
}

int TextureMaterialWithOpacity::compare(const AbstractEffect *o) const
{
    Q_ASSERT(o && type() == o->type());
    const TextureMaterialWithOpacity *other = static_cast<const TextureMaterialWithOpacity *>(o);
    if (int diff = m_texture->textureId() - other->texture()->textureId())
        return diff;
    if (int diff = int(m_linear_filtering) - int(other->m_linear_filtering))
        return diff;
    return int(other->m_opacity < m_opacity) - int(m_opacity < other->m_opacity);
}

void TextureMaterialWithOpacity::setTexture(const QSGTextureRef &texture, bool opaque)
{
    m_texture = texture;
    m_opaque = opaque;
    setFlags(opaque && m_opacity == 1 ? Flag(0) : Blending);
}

AbstractEffectProgram *TextureMaterialWithOpacity::createProgram() const
{
    return new TextureMaterialWithOpacityData;
}

void TextureMaterialWithOpacityData::updateEffectState(Renderer *renderer, AbstractEffect *newEffect, AbstractEffect *oldEffect)
{
    TextureMaterialData::updateEffectState(renderer, newEffect, oldEffect);

    Q_ASSERT(oldEffect == 0 || newEffect->type() == oldEffect->type());
    TextureMaterialWithOpacity *tx = static_cast<TextureMaterialWithOpacity *>(newEffect);
    TextureMaterialWithOpacity *oldTx = static_cast<TextureMaterialWithOpacity *>(oldEffect);

    if (oldTx == 0 || tx->opacity() != oldTx->opacity())
        m_program.setUniformValue(m_opacity_id, (GLfloat) tx->opacity());
}

void TextureMaterialWithOpacityData::initialize()
{
    TextureMaterialData::initialize();
    m_opacity_id = m_program.uniformLocation("opacity");
}
