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

#include "qsgtexturematerial_p.h"

#include <qglshaderprogram.h>

QT_BEGIN_NAMESPACE

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


const char *QSGTextureMaterialShader::vertexShader() const
{
    return qt_scenegraph_texture_material_vertex_code;
}

const char *QSGTextureMaterialShader::fragmentShader() const
{
    return qt_scenegraph_texture_material_fragment;
}

QSGMaterialType QSGTextureMaterialShader::type;

char const *const *QSGTextureMaterialShader::attributeNames() const
{
    static char const *const attr[] = { "qt_VertexPosition", "qt_VertexTexCoord", 0 };
    return attr;
}

void QSGTextureMaterialShader::initialize()
{
    m_matrix_id = m_program.uniformLocation("qt_Matrix");
}

void QSGTextureMaterialShader::updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect)
{
    Q_ASSERT(oldEffect == 0 || newEffect->type() == oldEffect->type());
    QSGTextureMaterial *tx = static_cast<QSGTextureMaterial *>(newEffect);
    QSGTextureMaterial *oldTx = static_cast<QSGTextureMaterial *>(oldEffect);

    QSGTexture *t = tx->texture().texture();

    t->setFiltering(tx->linearFiltering() ? QSGTexture::Linear : QSGTexture::Nearest);
    t->setHorizontalWrapMode(tx->clampToEdge() ? QSGTexture::ClampToEdge : QSGTexture::Repeat);
    t->setVerticalWrapMode(tx->clampToEdge() ? QSGTexture::ClampToEdge : QSGTexture::Repeat);

    if (oldTx == 0 || oldTx->texture()->textureId() != t->textureId())
        t->bind();
    else
        t->updateBindOptions();

    if (state.isMatrixDirty())
        m_program.setUniformValue(m_matrix_id, state.combinedMatrix());
}


QSGMaterialType *QSGTextureMaterial::type() const
{
    return &QSGTextureMaterialShader::type;
}

QSGMaterialShader *QSGTextureMaterial::createShader() const
{
    return new QSGTextureMaterialShader;
}


void QSGTextureMaterial::setTexture(const QSGTextureRef &texture, bool opaque)
{
    m_texture = texture;
    m_opaque = opaque;
    setFlag(Blending, !opaque);
}


int QSGTextureMaterial::compare(const QSGMaterial *o) const
{
    Q_ASSERT(o && type() == o->type());
    const QSGTextureMaterial *other = static_cast<const QSGTextureMaterial *>(o);
    if (int diff = m_texture->textureId() - other->texture()->textureId())
        return diff;
    return int(m_linear_filtering) - int(other->m_linear_filtering);
}

bool QSGTextureMaterial::is(const QSGMaterial *effect)
{
    return effect->type() == &QSGTextureMaterialShader::type;
}

// QSGTextureMaterialWithOpacity

static const char qt_scenegraph_texture_material_opacity_fragment[] =
    "varying highp vec2 qt_TexCoord;                       \n"
    "uniform sampler2D qt_Texture;                         \n"
    "uniform lowp float opacity;                        \n"
    "void main() {                                      \n"
    "    gl_FragColor = texture2D(qt_Texture, qt_TexCoord) * opacity; \n"
    "}";

class TextureMaterialWithOpacityShader : public QSGTextureMaterialShader
{
public:
    virtual void updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect);
    virtual void initialize();

    static QSGMaterialType type;

protected:
    virtual const char *fragmentShader() const { return qt_scenegraph_texture_material_opacity_fragment; }

    int m_opacity_id;
};
QSGMaterialType TextureMaterialWithOpacityShader::type;

bool QSGTextureMaterialWithOpacity::is(const QSGMaterial *effect)
{
    return effect->type() == &TextureMaterialWithOpacityShader::type;
}

QSGMaterialType *QSGTextureMaterialWithOpacity::type() const
{
    return &TextureMaterialWithOpacityShader::type;
}

QSGMaterialShader *QSGTextureMaterialWithOpacity::createShader() const
{
    return new TextureMaterialWithOpacityShader;
}

void TextureMaterialWithOpacityShader::updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect)
{
    Q_ASSERT(oldEffect == 0 || newEffect->type() == oldEffect->type());
    if (state.isOpacityDirty())
        m_program.setUniformValue(m_opacity_id, state.opacity());

    QSGTextureMaterialShader::updateState(state, newEffect, oldEffect);
}

void TextureMaterialWithOpacityShader::initialize()
{
    QSGTextureMaterialShader::initialize();
    m_opacity_id = m_program.uniformLocation("opacity");
}

QT_END_NAMESPACE
