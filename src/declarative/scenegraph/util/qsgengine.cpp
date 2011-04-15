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

#include "qsgengine.h"

#include <private/qsgtexture_p.h>

QT_BEGIN_NAMESPACE

class QSGEnginePrivate : public QObjectPrivate
{
public:
    QSGContext *context;
};


/*!
    \class QSGEngine
    \brief The QSGEngine class serves as a generic entry point into scene graph specific APIs.

    The QSGEngine class provides factory functions for creating textures. Though the user
    can implement any type of texture using the abstract QSGTexture class, the QSGEngine
    class provides some convenience for the default usecases. This also allows the scene
    graph to apply hardware specific optimzations.
 */

/*!
    Constructs a new QSGengine
 */
QSGEngine::QSGEngine(QObject *parent) :
    QObject(*(new QSGEnginePrivate), parent)
{
}


QSGEngine::~QSGEngine()
{
}

/*!
    \enum TextureOption

    The TextureOption enums are used to customize a texture is wrapped.

    \value TextureHasAlphaChannel The texture has an alpha channel and should
    be drawn using blending.

    \value TextureHasMipmaps The texture has mipmaps and can be drawn with
    mipmapping enabled.

    \value TextureOwnsGLTexture The texture object owns the texture id and
    will delete the GL texture when the texture object is deleted.

 */

/*!
    Creates a new QSGTexture from the supplied \a image. If the image has an
    alpha channel, the corresponding texture will have an alpha channel.

    The caller of the function is responsible for deleting the returned texture.

    The actual GL texture will be deleted when the texture object is deleted.
 */

QSGTexture *QSGEngine::createTextureFromImage(const QImage &image) const
{
    Q_D(const QSGEngine);
    return d->context->createTexture(image);
}


/*!
    Creates a new QSGTexture object from an existing GL texture \a id.

    The caller of the function is responsible for deleting the returned texture.

    Use \a options to customize the texture attributes.
 */
QSGTexture *QSGEngine::createTextureFromId(uint id, const QSize &size, TextureOptions options) const
{
    QSGPlainTexture *texture = new QSGPlainTexture();
    texture->setTextureId(id);
    texture->setHasAlphaChannel(options & TextureHasAlphaChannel);
    texture->setHasMipmaps(options & TextureHasMipmaps);
    texture->setOwnsTexture(options & TextureOwnsGLTexture);
    texture->setTextureSize(size);
    return texture;
}


void QSGEngine::setContext(QSGContext *context)
{
    Q_D(QSGEngine);
    d->context = context;
}

QT_END_NAMESPACE
