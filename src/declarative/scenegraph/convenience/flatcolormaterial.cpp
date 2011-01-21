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

#include "flatcolormaterial.h"

#include <qglshaderprogram.h>

class FlatColorMaterialData : public AbstractShaderEffectProgram
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
    int m_color_id;
};

AbstractEffectType FlatColorMaterialData::type;

const char *FlatColorMaterialData::vertexShader() const {
    return
        "attribute highp vec4 vCoord;                   \n"
        "uniform highp mat4 matrix;                     \n"
        "void main() {                                  \n"
        "    gl_Position = matrix * vCoord;             \n"
        "}";
}

const char *FlatColorMaterialData::fragmentShader() const {
    return
        "uniform lowp vec4 color;                       \n"
        "void main() {                                  \n"
        "    gl_FragColor = color;                      \n"
        "}";
}

const AbstractShaderEffectProgram::Attributes FlatColorMaterialData::attributes() const {
    static const QGL::VertexAttribute ids[] = { QGL::Position, QGL::VertexAttribute(-1) };
    static const char *const names[] = { "vCoord", 0 };
    static const Attributes attr = { ids, names };
    return attr;
}

void FlatColorMaterialData::initialize()
{
    AbstractShaderEffectProgram::initialize();
    m_matrix_id = m_program.uniformLocation("matrix");
    m_color_id = m_program.uniformLocation("color");
}

void FlatColorMaterialData::updateRendererState(Renderer *renderer, Renderer::Updates updates)
{
    if (updates & Renderer::UpdateMatrices)
        m_program.setUniformValue(m_matrix_id, renderer->combinedMatrix());
}

void FlatColorMaterialData::updateEffectState(Renderer *renderer, AbstractEffect *newEffect, AbstractEffect *oldEffect)
{
    Q_UNUSED(renderer)
    Q_ASSERT(oldEffect == 0 || newEffect->type() == oldEffect->type());

    FlatColorMaterial *oldMaterial = static_cast<FlatColorMaterial *>(oldEffect);
    FlatColorMaterial *newMaterial = static_cast<FlatColorMaterial *>(newEffect);

    const QColor &c = newMaterial->color();
    if (oldMaterial == 0
            || c != oldMaterial->color()
            || newMaterial->opacity() != oldMaterial->opacity()) {
        QVector4D v(c.redF() * c.alphaF() * newMaterial->opacity(),
                    c.greenF() * c.alphaF() * newMaterial->opacity(),
                    c.blueF() * c.alphaF() * newMaterial->opacity(),
                    c.alphaF() * newMaterial->opacity());
        m_program.setUniformValue(m_color_id, v);
    }
}


FlatColorMaterial::FlatColorMaterial() : m_color(Qt::white), m_opacity(1.0)
{
}

void FlatColorMaterial::setOpacity(qreal opacity)
{
    m_opacity = opacity;
    setFlags(m_color.alpha() == 0xff && m_opacity >= 1.0 ? Flags(0) : Blending);
}

void FlatColorMaterial::setColor(const QColor &color)
{
    m_color = color;
    setFlags(m_color.alpha() == 0xff && m_opacity >= 1.0 ? Flags(0) : Blending);
}


AbstractEffectType *FlatColorMaterial::type() const
{
    return &FlatColorMaterialData::type;
}

AbstractEffectProgram *FlatColorMaterial::createProgram() const
{
    return new FlatColorMaterialData;
}

int FlatColorMaterial::compare(const AbstractEffect *o) const
{
    Q_ASSERT(o && type() == o->type());
    const FlatColorMaterial *other = static_cast<const FlatColorMaterial *>(o);
    QRgb c1 = m_color.rgba();
    QRgb c2 = other->m_color.rgba();
    return int(c2 < c1) - int(c1 < c2);
}

bool FlatColorMaterial::is(const AbstractEffect *effect)
{
    return effect->type() == &FlatColorMaterialData::type;
}

