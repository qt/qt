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
#include <private/qsgrenderer_p.h>

QT_BEGIN_NAMESPACE

class QSGEnginePrivate : public QObjectPrivate
{
public:
    QSGEnginePrivate()
        : context(0)
        , clearBeforeRender(true)
    {
    }

    QSGContext *context;

    bool clearBeforeRender;
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
    \fn void QSGEngine::beforeRendering()

    This signal is emitted before the scene starts rendering.

    Combined with the modes for clearing the background, this option
    can be used to paint using raw GL under QML content.

    The GL context used for rendering the scene graph will be bound
    at this point.
*/

/*!
    \fn void QSGEngine::afterRendering()

    This signal is emitted after the scene has completed rendering, before swapbuffers is called.

    This signal can be used to paint using raw GL on top of QML content,
    or to do screen scraping of the current frame buffer.

    The GL context used for rendering the scene graph will be bound at this point.
 */



/*!
    Sets weither the scene graph rendering of QML should clear the color buffer
    before it starts rendering to \a enbled.

    By disabling clearing of the color buffer, it is possible to do GL painting
    under the scene graph.

    The color buffer is cleared by default.
 */

void QSGEngine::setClearBeforeRendering(bool enabled)
{
    Q_D(QSGEngine);
    d->clearBeforeRender = enabled;
    if (d->clearBeforeRender) {
        d->context->renderer()->setClearMode(QSGRenderer::ClearDepthBuffer
                                             | QSGRenderer::ClearColorBuffer);
    } else {
        d->context->renderer()->setClearMode(QSGRenderer::ClearDepthBuffer);
    }
}



/*!
    Returns weither clearing of the color buffer is done before rendering or not.
 */

bool QSGEngine::clearBeforeRendering() const
{
    Q_D(const QSGEngine);
    return d->clearBeforeRender;
}



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



/*!
    \internal

    Sets the scene graph context of this engine to context.

    The context will be set by the QSGcontext::initialize() function,
    as part of constructing the engine object.
 */

void QSGEngine::setContext(QSGContext *context)
{
    Q_D(QSGEngine);
    d->context = context;
}



/*!
    Sets the background color of the scene graph to \a color.

    Changing the clear color has no effect when clearing before rendering is
    disabled.
 */

void QSGEngine::setClearColor(const QColor &color)
{
    d_func()->context->renderer()->setClearColor(color);
}



/*!
    Returns the background color of the scene graph
 */

QColor QSGEngine::clearColor() const
{
    return d_func()->context->renderer()->clearColor();
}

QT_END_NAMESPACE
