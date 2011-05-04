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

#ifndef QSGTEXTURE_P_H
#define QSGTEXTURE_P_H

#include <private/qobject_p.h>

#include <QtOpenGL/qgl.h>

#include "qsgtexture.h"
#include <private/qsgcontext_p.h>

QT_BEGIN_NAMESPACE

class QSGTexturePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QSGTexture);
public:
    QSGTexturePrivate();

    uint wrapChanged : 1;
    uint filteringChanged : 1;

    uint horizontalWrap : 1;
    uint verticalWrap : 1;
    uint mipmapMode : 2;
    uint filterMode : 2;
};

class Q_DECLARATIVE_EXPORT QSGPlainTexture : public QSGTexture
{
    Q_OBJECT
public:
    QSGPlainTexture();
    virtual ~QSGPlainTexture();

    void setOwnsTexture(bool owns) { m_owns_texture = owns; }
    bool ownsTexture() const { return m_owns_texture; }

    void setTextureId(int id);
    int textureId() const { return m_texture_id; }

    void setTextureSize(const QSize &size) { m_texture_size = size; }
    QSize textureSize() const { return m_texture_size; }

    void setHasAlphaChannel(bool alpha) { m_has_alpha = alpha; }
    bool hasAlphaChannel() const { return m_has_alpha; }

    void setHasMipmaps(bool mm);
    bool hasMipmaps() const { return m_has_mipmaps; }

    void setImage(const QImage &image);

    virtual void bind();

protected:
    QImage m_image;

    GLuint m_texture_id;
    QSize m_texture_size;

    uint m_has_alpha : 1;
    uint m_has_mipmaps : 1;
    uint m_dirty_texture : 1;
    uint m_dirty_bind_options : 1;
    uint m_owns_texture : 1;
    uint m_mipmaps_generated : 1;
};

QT_END_NAMESPACE

#endif // QSGTEXTURE_P_H
