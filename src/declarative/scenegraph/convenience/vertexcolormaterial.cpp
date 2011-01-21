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

#include "vertexcolormaterial.h"

#include <qglshaderprogram.h>

class VertexColorMaterialData : public AbstractShaderEffectProgram
{
public:
    virtual void updateRendererState(Renderer *renderer, Renderer::Updates updates);
    virtual void updateEffectState(Renderer *renderer, AbstractEffect *newEffect, AbstractEffect *oldEffect);

    static AbstractEffectType type;

private:
    virtual void initialize();
    virtual const char *vertexShader() const;
    virtual const char *fragmentShader() const;
    virtual const Attributes attributes() const;

    int m_matrix_id;
    int m_opacity_id;
};

AbstractEffectType VertexColorMaterialData::type;

const char *VertexColorMaterialData::vertexShader() const {
    return
        "attribute highp vec4 vertexCoord;              \n"
        "attribute highp vec4 vertexColor;              \n"
        "uniform highp mat4 matrix;                     \n"
        "uniform highp float opacity;                   \n"
        "varying lowp vec4 color;                       \n"
        "void main() {                                  \n"
        "    gl_Position = matrix * vertexCoord;        \n"
        "    color = vertexColor * opacity;             \n"
        "}";
}

const char *VertexColorMaterialData::fragmentShader() const {
    return
        "varying lowp vec4 color;                       \n"
        "void main() {                                  \n"
        "    gl_FragColor = color;                      \n"
        "}";
}

const AbstractShaderEffectProgram::Attributes VertexColorMaterialData::attributes() const {
    static const QGL::VertexAttribute ids[] = { QGL::Position, QGL::Color, QGL::VertexAttribute(-1) };
    static const char *const names[] = { "vertexCoord", "vertexColor", 0 };
    static const Attributes attr = { ids, names };
    return attr;
}

void VertexColorMaterialData::initialize()
{
    AbstractShaderEffectProgram::initialize();
    m_matrix_id = m_program.uniformLocation("matrix");
    m_opacity_id = m_program.uniformLocation("opacity");
}

void VertexColorMaterialData::updateRendererState(Renderer *renderer, Renderer::Updates updates)
{
    if (updates & Renderer::UpdateMatrices)
        m_program.setUniformValue(m_matrix_id, renderer->combinedMatrix());
}

void VertexColorMaterialData::updateEffectState(Renderer *, AbstractEffect *newEffect, AbstractEffect *oldEffect)
{
    Q_ASSERT(oldEffect == 0 || newEffect->type() == oldEffect->type());
    VertexColorMaterial *material = static_cast<VertexColorMaterial *>(newEffect);
    VertexColorMaterial *oldMaterial = static_cast<VertexColorMaterial *>(oldEffect);

    if (oldMaterial == 0 || oldMaterial->opacity() != material->opacity())
        m_program.setUniformValue(m_opacity_id, GLfloat(material->opacity()));
}


VertexColorMaterial::VertexColorMaterial(bool opaque) : m_opacity(1), m_opaque(opaque)
{
    setFlags(opaque ? Flags(0) : Blending);
}

void VertexColorMaterial::setOpaque(bool opaque)
{
    setFlags(m_opacity == 1 && opaque ? Flags(0) : Blending);
    m_opaque = opaque;
}

void VertexColorMaterial::setOpacity(qreal opacity)
{
    setFlags(opacity == 1 && m_opaque ? Flags(0) : Blending);
    m_opacity = opacity;
}

AbstractEffectType *VertexColorMaterial::type() const
{
    return &VertexColorMaterialData::type;
}

AbstractEffectProgram *VertexColorMaterial::createProgram() const
{
    return new VertexColorMaterialData;
}

int VertexColorMaterial::compare(const AbstractEffect *o) const
{
    Q_ASSERT(o && type() == o->type());
    const VertexColorMaterial *other = static_cast<const VertexColorMaterial *>(o);
    qreal o1 = m_opacity, o2 = other->m_opacity;
    return int(o2 < o1) - int(o1 < o2);
}

bool VertexColorMaterial::is(const AbstractEffect *effect)
{
    return effect->type() == &VertexColorMaterialData::type;
}
