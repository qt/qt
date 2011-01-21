#include "mactexturemanager.h"

#include <qevent.h>

/*!
    Since we are using client storage which means asynchronous DMA
    uploads, we need to keep the bits around for the duration
    of the texture id. So we keep the image in along in the reference
 */
class QSGMacTextureReference : public TextureReference
{
public:
    void timerEvent(QTimerEvent *t) {
        setStatus(TextureReference::Uploaded);
        killTimer(t->timerId());
        --inProgress;
    }

    QImage image;
    bool big;

    static int inProgress;
};

int QSGMacTextureReference::inProgress = 0;


QSGMacTextureManager::QSGMacTextureManager()
{
}


const QSGTextureRef &QSGMacTextureManager::requestUploadedTexture(const QImage &image, UploadHints hints, QObject *listener, const char *slot)
{
    // Client storage will most likely fail when mipmapping is used..
    if (hints & TextureManager::GenerateMipmapUploadHint)
        return TextureManager::requestUploadedTexture(image, hints, listener, slot);

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, 1);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_STORAGE_HINT_APPLE, GL_STORAGE_SHARED_APPLE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_BGRA,
                 GL_UNSIGNED_INT_8_8_8_8_REV, image.constBits());

    glBindTexture(GL_TEXTURE_2D, 0);
    glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, 0);

    QSGMacTextureReference *texture = new QSGMacTextureReference;
    if (listener && slot) {
        QObject::connect(texture, SIGNAL(statusChanged(int)), listener, slot);
    }
    texture->image = image;
    texture->setTextureId(id);
    texture->setTextureSize(image.size());
    texture->setOwnsTexture(true);
    texture->setAlphaChannel(image.hasAlphaChannel());

    if (hints & TextureManager::SynchronousUploadHint) {
        texture->setStatus(TextureReference::Uploaded);
    } else {
        // Delay "big" images
        int byteSize = image.height() * image.bytesPerLine();
        if (byteSize > 500000) {
            texture->setStatus(TextureReference::Uploading);
            ++QSGMacTextureReference::inProgress;
            int delay = byteSize / 50000 * QSGMacTextureReference::inProgress;
            texture->startTimer(delay);
        } else {
            texture->setStatus(TextureReference::Uploaded);
        }
    }

    return texture;
}
