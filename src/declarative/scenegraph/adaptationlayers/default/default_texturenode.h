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


#ifndef DEFAULT_PIXMAPNODE_H
#define DEFAULT_PIXMAPNODE_H

#include "adaptationlayer.h"

#include "texturematerial.h"

class TextureProviderMaterial : public AbstractMaterial
{
public:
    TextureProviderMaterial()
        : m_texture(0)
    {
    }

    virtual AbstractMaterialType *type() const;
    virtual AbstractMaterialShader *createShader() const;
    virtual int compare(const AbstractMaterial *other) const;

    void setTexture(QSGTextureProvider *texture);
    QSGTextureProvider *texture() const { return m_texture; }

    static bool is(const AbstractMaterial *effect);

protected:
    QSGTextureProvider *m_texture;
};


class TextureProviderMaterialWithOpacity : public TextureProviderMaterial
{
public:
    TextureProviderMaterialWithOpacity() { }

    virtual AbstractMaterialType *type() const;
    virtual AbstractMaterialShader *createShader() const;
    void setTexture(QSGTextureProvider *texture);

    static bool is(const AbstractMaterial *effect);
};


class DefaultTextureNode : public TextureNodeInterface
{
public:
    DefaultTextureNode();
    virtual void setTargetRect(const QRectF &rect);
    virtual void setSourceRect(const QRectF &rect);
    virtual void setTexture(QSGTextureProvider *texture);
    virtual void update();

    virtual void preprocess();

private:
    void updateGeometry();

    TextureProviderMaterial m_material;
    TextureProviderMaterialWithOpacity m_materialO;

    uint m_dirtyGeometry : 1;

    QSGGeometry m_geometry;
};

#endif
