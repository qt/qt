#include "qsgimagetextureprovider_p.h"

QSGImageTextureProvider::QSGImageTextureProvider(QObject *parent)
    : QSGTextureProvider(parent)
{
}

void QSGImageTextureProvider::setImage(const QImage &image)
{
    m_image = image;
    m_texture = QSGTextureRef();
}

#ifdef QT_OPENGL_ES
static void swizzleBGRAToRGBA(QImage *image)
{
    const int width = image->width();
    const int height = image->height();
    for (int i = 0; i < height; ++i) {
        uint *p = (uint *) image->scanLine(i);
        for (int x = 0; x < width; ++x)
            p[x] = ((p[x] << 16) & 0xff0000) | ((p[x] >> 16) & 0xff) | (p[x] & 0xff00ff00);
    }
}
#endif

void QSGImageTextureProvider::updateTexture()
{
    if (m_texture.isReady())
        return;

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    QImage i = m_image.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    int w = i.width();
    int h = i.height();
    int bpl = i.bytesPerLine();

#ifdef QT_OPENGL_ES
    QSGTextureManager::swizzleBGRAToRGBA(&i);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, i.constBits());
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, i.constBits());
#endif

    QSGTexture *t = new QSGTexture();
    t->setTextureId(id);
    t->setTextureSize(QSize(w, h));
    t->setOwnsTexture(true);
    t->setAlphaChannel(m_image.hasAlphaChannel());
    t->setStatus(QSGTexture::Ready);

    m_texture = QSGTextureRef(t);
}


QSGTextureRef QSGImageTextureProvider::texture()
{
    return m_texture;
}


QSize QSGImageTextureProvider::textureSize() const
{
    return m_image.size();
}

