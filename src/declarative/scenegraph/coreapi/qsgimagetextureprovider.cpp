#include "qsgimagetextureprovider_p.h"

QSGImageTextureProvider::QSGImageTextureProvider(QObject *parent)
    : QSGTextureProvider(parent)
    , m_dirty_texture(false)
{
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


void QSGImageTextureProvider::setImage(const QImage &image)
{
    m_image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
#ifdef QT_OPENGL_ES
    QSGTextureManager::swizzleBGRAToRGBA(&m_image);
#endif
    m_dirty_texture = true;
}

void QSGImageTextureProvider::updateTexture()
{
    if (!m_dirty_texture)
        return;

    m_dirty_texture = false;

    // Deref textures first to aid in releasing memory before allocating more.
    m_texture = QSGTextureRef();

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    int w = m_image.width();
    int h = m_image.height();

#ifdef QT_OPENGL_ES
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.constBits());
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_image.constBits());
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

