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

#include "qsgtextureprovider.h"

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

QT_BEGIN_NAMESPACE

/*!
    \class QSGTextureProvider
    \brief The QSGTextureProvider class encapsulates texture based entities in QML.

    The QSGTextureProvider objects are live primarily on the QML thread, aside from
    the functions that are specified otherwise.
 */

QSGTextureProvider::QSGTextureProvider(QObject *parent)
    : QObject(parent)
    , m_opaque(false)
    , m_hWrapMode(ClampToEdge)
    , m_vWrapMode(ClampToEdge)
    , m_filtering(Nearest)
    , m_mipmap(None)
{
}


/*!
    Propegate the state in the texture provider to the texture and
    binds the texture.
 */
void QSGTextureProvider::bind(QSGTexture *oldTexture)
{
    QSGTexture *t = texture().texture();
    t->setFiltering((QSGTexture::Filtering) filtering());
    t->setMipmapFiltering((QSGTexture::Filtering) mipmap());
    t->setHorizontalWrapMode((QSGTexture::WrapMode) horizontalWrapMode());
    t->setVerticalWrapMode((QSGTexture::WrapMode) verticalWrapMode());
    if (t != oldTexture)
        t->bind();
    else
        t->updateBindOptions();
}

/*!
    \fn void QSGTextureProvider::updateTexture()

    This function will be called on the renderer thread when the textures should
    be updated.

    The funciton might be called even though the textureChanged() signal has
    not been emitted, so implementations should consider doing some caching.
 */

/*!
    Returns true if this texture provider contains a texture that might change content
    over time.

    The default implementation returns false, meaning the texture may change from
    frame to frame.
 */
bool QSGTextureProvider::isStaticTexture() const
{
    return false;
}

GLint QSGTextureProvider::glTextureWrapS() const
{
    return m_hWrapMode == Repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
}

GLint QSGTextureProvider::glTextureWrapT() const
{
    return m_vWrapMode == Repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
}

GLint QSGTextureProvider::glMinFilter() const
{
    bool linear = m_filtering == Linear;
    switch (m_mipmap) {
    case Nearest:
        return linear ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST;
    case Linear:
        return linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR;
    default:
        return linear ? GL_LINEAR : GL_NEAREST;
    }
}

GLint QSGTextureProvider::glMagFilter() const
{
    return m_filtering == Linear ? GL_LINEAR : GL_NEAREST;
}


QT_END_NAMESPACE
