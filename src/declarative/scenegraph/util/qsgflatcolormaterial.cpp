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

#include "qsgflatcolormaterial.h"

#include <qglshaderprogram.h>

QT_BEGIN_NAMESPACE

class FlatColorMaterialShader : public QSGMaterialShader
{
public:
    virtual void updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect);
    virtual char const *const *attributeNames() const;

    static QSGMaterialType type;

private:
    virtual void initialize();
    virtual const char *vertexShader() const;
    virtual const char *fragmentShader() const;

    int m_matrix_id;
    int m_color_id;
};

QSGMaterialType FlatColorMaterialShader::type;

void FlatColorMaterialShader::updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect)
{
    Q_ASSERT(oldEffect == 0 || newEffect->type() == oldEffect->type());

    QSGFlatColorMaterial *oldMaterial = static_cast<QSGFlatColorMaterial *>(oldEffect);
    QSGFlatColorMaterial *newMaterial = static_cast<QSGFlatColorMaterial *>(newEffect);

    const QColor &c = newMaterial->color();

    if (oldMaterial == 0 || c != oldMaterial->color() || state.isOpacityDirty()) {
        float opacity = state.opacity();
        QVector4D v(c.redF() * c.alphaF() * opacity,
                    c.greenF() * c.alphaF() * opacity,
                    c.blueF() * c.alphaF() * opacity,
                    c.alphaF() * opacity);
        m_program.setUniformValue(m_color_id, v);
    }

    if (state.isMatrixDirty())
        m_program.setUniformValue(m_matrix_id, state.combinedMatrix());
}

char const *const *FlatColorMaterialShader::attributeNames() const
{
    static char const *const attr[] = { "vCoord", 0 };
    return attr;
}

void FlatColorMaterialShader::initialize()
{
    m_matrix_id = m_program.uniformLocation("matrix");
    m_color_id = m_program.uniformLocation("color");
}

const char *FlatColorMaterialShader::vertexShader() const {
    return
        "attribute highp vec4 vCoord;                   \n"
        "uniform highp mat4 matrix;                     \n"
        "void main() {                                  \n"
        "    gl_Position = matrix * vCoord;             \n"
        "}";
}

const char *FlatColorMaterialShader::fragmentShader() const {
    return
        "uniform lowp vec4 color;                       \n"
        "void main() {                                  \n"
        "    gl_FragColor = color;                      \n"
        "}";
}


QSGFlatColorMaterial::QSGFlatColorMaterial() : m_color(QColor(255, 255, 255))
{
}

void QSGFlatColorMaterial::setColor(const QColor &color)
{
    m_color = color;
    setFlag(Blending, m_color.alpha() != 0xff);
}


QSGMaterialType *QSGFlatColorMaterial::type() const
{
    return &FlatColorMaterialShader::type;
}

QSGMaterialShader *QSGFlatColorMaterial::createShader() const
{
    return new FlatColorMaterialShader;
}

QT_END_NAMESPACE
