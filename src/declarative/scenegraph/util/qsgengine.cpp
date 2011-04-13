#include "qsgengine.h"

#include <private/qsgtexture_p.h>

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
    QObject(parent)
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
