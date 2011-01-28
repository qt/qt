#include "qsgmactexturemanager_mac_p.h"
#include "qsgtexturemanager_p.h"

/*!
    Since we are using client storage which means asynchronous DMA
    uploads, we need to keep the bits around for the duration
    of the texture id. So we keep the image in along in the reference
 */
class QSGMacTexture: public QSGTexture
{
public:
    QImage image;
};


QSGMacTextureManager::QSGMacTextureManager() :
    QSGTextureManager()
{
}

QSGTextureRef QSGMacTextureManager::upload(const QImage &image)
{
    // Check if the image is already uploaded and cached
    QSGTextureCacheKey key = { image.cacheKey() };
    QSGTexture *cached = static_cast<QSGTextureManagerPrivate *>(d_ptr.data())->cache.value(key);
    if (cached)
        return QSGTextureRef(cached);

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, 1);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_STORAGE_HINT_APPLE, GL_STORAGE_SHARED_APPLE);

    QImage converted = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_BGRA,
                 GL_UNSIGNED_INT_8_8_8_8_REV, converted.constBits());

    glBindTexture(GL_TEXTURE_2D, 0);
    glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, 0);

    QSGMacTexture *texture = new QSGMacTexture;
    texture->image = converted;
    texture->setTextureId(id);
    texture->setTextureSize(image.size());
    texture->setOwnsTexture(true);
    texture->setAlphaChannel(image.hasAlphaChannel());
    texture->setStatus(QSGTexture::Ready);

    static_cast<QSGTextureManagerPrivate *>(d_ptr.data())->cache.insert(key, texture);

    return texture;
}
