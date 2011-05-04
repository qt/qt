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

#include "qsgmaterial.h"
#include <qsgtexture.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


class Q_DECLARATIVE_EXPORT QSGTextureMaterial : public QSGMaterial
{
public:
    QSGTextureMaterial();

    virtual QSGMaterialType *type() const;
    virtual QSGMaterialShader *createShader() const;
    virtual int compare(const QSGMaterial *other) const;

    void setTexture(QSGTexture *texture);
    QSGTexture *texture() const { return m_texture; }

    void setMipmapFiltering(QSGTexture::Filtering filtering) { m_mipmap_filtering = filtering; }
    QSGTexture::Filtering mipmapFiltering() const { return (QSGTexture::Filtering) m_mipmap_filtering; }

    void setFiltering(QSGTexture::Filtering filtering) { m_filtering = filtering; }
    QSGTexture::Filtering filtering() const { return (QSGTexture::Filtering)  m_filtering; }

    void setHorizontalWrapMode(QSGTexture::WrapMode mode) { m_horizontal_wrap = mode; }
    QSGTexture::WrapMode horizontalWrapMode() const { return (QSGTexture::WrapMode) m_horizontal_wrap; }

    void setVerticalWrapMode(QSGTexture::WrapMode mode) { m_vertical_wrap = mode; }
    QSGTexture::WrapMode verticalWrapMode() const { return (QSGTexture::WrapMode) m_vertical_wrap; }

protected:
    QSGTexture *m_texture;

    uint m_filtering: 2;
    uint m_mipmap_filtering: 2;
    uint m_vertical_wrap: 1;
    uint m_horizontal_wrap : 1;
};


class Q_DECLARATIVE_EXPORT QSGTextureMaterialWithOpacity : public QSGTextureMaterial
{
public:
    virtual QSGMaterialType *type() const;
    virtual QSGMaterialShader *createShader() const;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // TEXTUREMATERIAL_H
