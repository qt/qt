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

#ifndef TEXTUREMATERIAL_H
#define TEXTUREMATERIAL_H

#include "material.h"
#include <qsgtexture.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class Q_DECLARATIVE_EXPORT TextureMaterial : public AbstractMaterial
{
public:
    TextureMaterial()
        : m_texture(0)
        , m_opaque(true)
        , m_linear_filtering(false)
        , m_clamp_to_edge(true)
    {
    }

    virtual AbstractMaterialType *type() const;
    virtual AbstractMaterialShader *createShader() const;
    virtual int compare(const AbstractMaterial *other) const;

    // ### gunnar: opaque -> alpha, as "hasAlphaChannel()" is what we normally use
    void setTexture(const QSGTextureRef &texture, bool opaque = false);
    const QSGTextureRef &texture() const { return m_texture; }

    void setLinearFiltering(bool linearFiltering) { m_linear_filtering = linearFiltering; }
    bool linearFiltering() const { return m_linear_filtering; }

    void setClampToEdge(bool clamp) { m_clamp_to_edge = clamp; }
    bool clampToEdge() const { return m_clamp_to_edge; }

    static bool is(const AbstractMaterial *effect);

protected:
    QSGTextureRef m_texture;
    bool m_opaque;
    bool m_linear_filtering;
    bool m_clamp_to_edge;
};

class Q_DECLARATIVE_EXPORT TextureMaterialShader : public AbstractMaterialShader
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


class Q_DECLARATIVE_EXPORT TextureMaterialWithOpacity : public TextureMaterial
{
public:
    virtual AbstractMaterialType *type() const;
    virtual AbstractMaterialShader *createShader() const;

    void setOpacity(qreal) {}
    qreal opacity() const { return 1; }

    static bool is(const AbstractMaterial *effect);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // TEXTUREMATERIAL_H
