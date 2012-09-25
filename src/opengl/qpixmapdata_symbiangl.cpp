/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qpixmap.h"
#include "qglframebufferobject.h"

#include <private/qpaintengine_raster_p.h>

#include "qpixmapdata_gl_p.h"

#include <private/qgl_p.h>
#include <private/qdrawhelper_p.h>
#include <private/qimage_p.h>
#include <private/qnativeimagehandleprovider_p.h>
#include <private/qfont_p.h>

#include <private/qpaintengineex_opengl2_p.h>

#include <qdesktopwidget.h>
#include <qfile.h>
#include <qimagereader.h>
#include <qbuffer.h>

#include <fbs.h>

#include "qgltexturepool_p.h"

QT_BEGIN_NAMESPACE

Q_OPENGL_EXPORT extern QGLWidget* qt_gl_share_widget();

class QGLSgImageTextureCleanup
{
public:
    QGLSgImageTextureCleanup(const QGLContext *context = 0) {}

    ~QGLSgImageTextureCleanup()
    {
        QList<qint64> keys = m_cache.keys();
        while(keys.size() > 0) {
            QGLPixmapData *data = m_cache.take(keys.takeAt(0));
            if (data)
                data->destroyTexture();
        }
    }

    static QGLSgImageTextureCleanup *cleanupForContext(const QGLContext *context);

    void insert(quint64 key, QGLPixmapData *data)
    {
        m_cache.insert(key, data);
    }

    void remove(quint64 key)
    {
        m_cache.take(key);
    }

private:

    QCache<qint64, QGLPixmapData> m_cache;
};

#if QT_VERSION >= 0x040800
Q_GLOBAL_STATIC(QGLContextGroupResource<QGLSgImageTextureCleanup>, qt_sgimage_texture_cleanup)
#else
static void qt_sgimage_texture_cleanup_free(void *data)
{
    delete reinterpret_cast<QGLSgImageTextureCleanup *>(data);
}
Q_GLOBAL_STATIC_WITH_ARGS(QGLContextResource, qt_sgimage_texture_cleanup, (qt_sgimage_texture_cleanup_free))
#endif

QGLSgImageTextureCleanup *QGLSgImageTextureCleanup::cleanupForContext(const QGLContext *context)
{
    QGLSgImageTextureCleanup *p = reinterpret_cast<QGLSgImageTextureCleanup *>(qt_sgimage_texture_cleanup()->value(context));
#if QT_VERSION < 0x040800
    if (!p) {
        QGLShareContextScope scope(context);
        qt_sgimage_texture_cleanup()->insert(context, p = new QGLSgImageTextureCleanup);
    }
#endif
    return p;
}

int qt_gl_pixmap_serial = 0;

QGLPixmapData::QGLPixmapData(PixelType type)
    : QPixmapData(type, OpenGLClass)
    , m_renderFbo(0)
    , m_engine(0)
    , m_ctx(0)
    , nativeImageHandleProvider(0)
    , nativeImageHandle(0)
#ifdef QT_SYMBIAN_SUPPORTS_SGIMAGE
    , m_sgImage(0)
#endif
    , m_dirty(false)
    , m_hasFillColor(false)
    , m_hasAlpha(false)
{
    setSerialNumber(++qt_gl_pixmap_serial);
}

QGLPixmapData::~QGLPixmapData()
{
#ifdef QT_SYMBIAN_SUPPORTS_SGIMAGE
    if (m_sgImage) {
        if (m_texture.id) {
            QGLSgImageTextureCleanup::cleanupForContext(m_ctx)->remove(m_texture.id);
            destroyTexture();
        }

        m_sgImage->Close();
        delete m_sgImage;
        m_sgImage = 0;
    }
#endif
    delete m_engine;
}

QPixmapData *QGLPixmapData::createCompatiblePixmapData() const
{
    return new QGLPixmapData(pixelType());
}

bool QGLPixmapData::isValid() const
{
    return w > 0 && h > 0;
}

bool QGLPixmapData::isValidContext(const QGLContext *ctx) const
{
    // On Symbian, we usually want to treat QGLPixmapData as
    // raster pixmap data because that's well known and tested
    // execution path which is used on other platforms as well.
    // That's why if source pixels are valid we return false
    // to simulate raster pixmaps. Only QPixmaps created from
    // SgImage will enable usage of QGLPixmapData.
#ifdef QT_SYMBIAN_SUPPORTS_SGIMAGE
    if (m_sgImage) {
        // SgImage texture
        if (ctx == m_ctx)
            return true;

        const QGLContext *share_ctx = qt_gl_share_widget()->context();
        return ctx == share_ctx || QGLContext::areSharing(ctx, share_ctx);
    }
#endif
    return false;
}

void QGLPixmapData::resize(int width, int height)
{
    if (width == w && height == h)
        return;

    if (width <= 0 || height <= 0) {
        width = 0;
        height = 0;
    }

    w = width;
    h = height;
    is_null = (w <= 0 || h <= 0);
    d = pixelType() == QPixmapData::PixmapType ? 32 : 1;

    destroyTexture();

    m_source = QVolatileImage();
    m_dirty = isValid();
    setSerialNumber(++qt_gl_pixmap_serial);
}

void QGLPixmapData::ensureCreated() const
{
    if (!m_dirty)
        return;

    m_dirty = false;

    if (nativeImageHandleProvider && !nativeImageHandle)
        const_cast<QGLPixmapData *>(this)->createFromNativeImageHandleProvider();

    QGLShareContextScope ctx(qt_gl_share_widget()->context());
    m_ctx = ctx;

#ifdef QT_SYMBIAN_SUPPORTS_SGIMAGE
    if (m_sgImage) {
        qt_resolve_eglimage_gl_extensions(ctx); // ensure initialized

        bool textureIsBound = false;
        GLuint newTextureId;

        EGLint imgAttr[] = { EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE };
        EGLImageKHR image = QEgl::eglCreateImageKHR(QEgl::display()
                                                , EGL_NO_CONTEXT
                                                , EGL_NATIVE_PIXMAP_KHR
                                                , (EGLClientBuffer)m_sgImage
                                                , imgAttr);

        glGenTextures(1, &newTextureId);
        glBindTexture( GL_TEXTURE_2D, newTextureId);

        if (image != EGL_NO_IMAGE_KHR) {
            glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, image);
            GLint err = glGetError();
            if (err == GL_NO_ERROR)
                textureIsBound = true;

            QEgl::eglDestroyImageKHR(QEgl::display(), image);
        }

        if (textureIsBound) {
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            m_texture.id = newTextureId;
            m_texture.boundPixmap = const_cast<QGLPixmapData*>(this);
            QGLSgImageTextureCleanup::cleanupForContext(m_ctx)->insert(m_texture.id, const_cast<QGLPixmapData*>(this));
        } else {
            qWarning("QGLPixmapData: Failed to create texture from a SgImage image of size %dx%d", w, h);
            glDeleteTextures(1, &newTextureId);
        }
    }
#endif
}


void QGLPixmapData::fromImage(const QImage &image,
                              Qt::ImageConversionFlags flags)
{
    QImage img = image;
    createPixmapForImage(img, flags, false);
}

void QGLPixmapData::fromImageReader(QImageReader *imageReader,
                                 Qt::ImageConversionFlags flags)
{
    QImage image = imageReader->read();
    if (image.isNull())
        return;

    createPixmapForImage(image, flags, true);
}

bool QGLPixmapData::fromFile(const QString &filename, const char *format,
                             Qt::ImageConversionFlags flags)
{
    if (pixelType() == QPixmapData::BitmapType)
        return QPixmapData::fromFile(filename, format, flags);
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.peek(64);
        bool alpha;
        if (m_texture.canBindCompressedTexture
                (data.constData(), data.size(), format, &alpha)) {
            resize(0, 0);
            data = file.readAll();
            file.close();
            QGLShareContextScope ctx(qt_gl_share_widget()->context());
            QSize size = m_texture.bindCompressedTexture
                (data.constData(), data.size(), format);
            if (!size.isEmpty()) {
                w = size.width();
                h = size.height();
                is_null = false;
                d = 32;
                m_hasAlpha = alpha;
                m_source = QVolatileImage();
                m_dirty = isValid();
                return true;
            }
            return false;
        }
    }

    QImage image = QImageReader(filename, format).read();
    if (image.isNull())
        return false;

    createPixmapForImage(image, flags, true);

    return !isNull();
}

bool QGLPixmapData::fromData(const uchar *buffer, uint len, const char *format,
                             Qt::ImageConversionFlags flags)
{
    bool alpha;
    const char *buf = reinterpret_cast<const char *>(buffer);
    if (m_texture.canBindCompressedTexture(buf, int(len), format, &alpha)) {
        resize(0, 0);
        QGLShareContextScope ctx(qt_gl_share_widget()->context());
        QSize size = m_texture.bindCompressedTexture(buf, int(len), format);
        if (!size.isEmpty()) {
            w = size.width();
            h = size.height();
            is_null = false;
            d = 32;
            m_hasAlpha = alpha;
            m_source = QVolatileImage();
            m_dirty = isValid();
            return true;
        }
    }

    QByteArray a = QByteArray::fromRawData(reinterpret_cast<const char *>(buffer), len);
    QBuffer b(&a);
    b.open(QIODevice::ReadOnly);
    QImage image = QImageReader(&b, format).read();
    if (image.isNull())
        return false;

    createPixmapForImage(image, flags, true);

    return !isNull();
}

QImage::Format QGLPixmapData::idealFormat(QImage &image, Qt::ImageConversionFlags flags)
{
    QImage::Format format = QImage::Format_RGB32;
    if (qApp->desktop()->depth() == 16)
        format = QImage::Format_RGB16;

    if (image.hasAlphaChannel()
        && ((flags & Qt::NoOpaqueDetection)
            || const_cast<QImage &>(image).data_ptr()->checkForAlphaPixels()))
        format = QImage::Format_ARGB32_Premultiplied;

    return format;
}

void QGLPixmapData::createPixmapForImage(QImage &image, Qt::ImageConversionFlags flags, bool inPlace)
{
    if (image.size() == QSize(w, h))
        setSerialNumber(++qt_gl_pixmap_serial);

    resize(image.width(), image.height());

    if (pixelType() == BitmapType) {
        QImage convertedImage = image.convertToFormat(QImage::Format_MonoLSB);
        if (image.format() == QImage::Format_MonoLSB)
            convertedImage.detach();

        m_source = QVolatileImage(convertedImage);

    } else {
        QImage::Format format = idealFormat(image, flags);

        if (inPlace && image.data_ptr()->convertInPlace(format, flags)) {
            m_source = QVolatileImage(image);
        } else {
            QImage convertedImage = image.convertToFormat(format);

            // convertToFormat won't detach the image if format stays the same.
            if (image.format() == format)
                convertedImage.detach();

            m_source = QVolatileImage(convertedImage);
        }
    }

    m_dirty = true;
    m_hasFillColor = false;

    m_hasAlpha = m_source.hasAlphaChannel();
    w = image.width();
    h = image.height();
    is_null = (w <= 0 || h <= 0);
    d = m_source.depth();

    destroyTexture();
}

bool QGLPixmapData::scroll(int dx, int dy, const QRect &rect)
{
    Q_UNUSED(dx);
    Q_UNUSED(dy);
    Q_UNUSED(rect);
    return false;
}

void QGLPixmapData::copy(const QPixmapData *data, const QRect &rect)
{
    QPixmapData::copy(data, rect);
}

void QGLPixmapData::fill(const QColor &color)
{
    if (!isValid())
        return;

    bool hasAlpha = color.alpha() != 255;
    if (hasAlpha && !m_hasAlpha) {
        if (m_texture.id) {
            destroyTexture();
            m_dirty = true;
        }
        m_hasAlpha = color.alpha() != 255;
    }

        forceToImage();

        if (m_source.depth() == 32) {
            m_source.fill(PREMUL(color.rgba()));

        } else if (m_source.depth() == 1) {
            if (color == Qt::color1)
                m_source.fill(1);
            else
                m_source.fill(0);
    }
}

bool QGLPixmapData::hasAlphaChannel() const
{
    return m_hasAlpha;
}

QImage QGLPixmapData::fillImage(const QColor &color) const
{
    QImage img;
    if (pixelType() == BitmapType) {
        img = QImage(w, h, QImage::Format_MonoLSB);

        img.setColorCount(2);
        img.setColor(0, QColor(Qt::color0).rgba());
        img.setColor(1, QColor(Qt::color1).rgba());

        if (color == Qt::color1)
            img.fill(1);
        else
            img.fill(0);
    } else {
        img = QImage(w, h,
                m_hasAlpha
                ? QImage::Format_ARGB32_Premultiplied
                : QImage::Format_RGB32);
        img.fill(PREMUL(color.rgba()));
    }
    return img;
}

extern QImage qt_gl_read_texture(const QSize &size, bool alpha_format, bool include_alpha);

QImage QGLPixmapData::toImage() const
{
    if (!isValid())
        return QImage();

    if (!m_source.isNull()) {
        // QVolatileImage::toImage() will make a copy always so no check
        // for active painting is needed.
        QImage img = m_source.toImage();
        if (img.format() == QImage::Format_MonoLSB) {
            img.setColorCount(2);
            img.setColor(0, QColor(Qt::color0).rgba());
            img.setColor(1, QColor(Qt::color1).rgba());
        }
        return img;
    } else if (m_dirty || m_hasFillColor) {
        return fillImage(m_fillColor);
    } else {
        ensureCreated();
    }

    QGLShareContextScope ctx(qt_gl_share_widget()->context());
    glBindTexture(GL_TEXTURE_2D, m_texture.id);
    return qt_gl_read_texture(QSize(w, h), true, true);
}

void QGLPixmapData::copyBackFromRenderFbo(bool keepCurrentFboBound) const
{
    // We don't use FBOs on Symbian
}

bool QGLPixmapData::useFramebufferObjects() const
{
    // We don't use FBOs on Symbian for now
    return false;
}

QPaintEngine* QGLPixmapData::paintEngine() const
{
    if (!isValid())
        return 0;

    // If the application wants to paint into the QPixmap, we first
    // force it to QImage format and then paint into that.
    // This is simpler than juggling multiple GL contexts.
    const_cast<QGLPixmapData *>(this)->forceToImage();

    if (m_hasFillColor) {
        m_source.fill(PREMUL(m_fillColor.rgba()));
        m_hasFillColor = false;
    }
    return m_source.paintEngine();
}

extern QRgb qt_gl_convertToGLFormat(QRgb src_pixel, GLenum texture_format);

GLuint QGLPixmapData::bind(bool copyBack) const
{
    ensureCreated();

    GLuint id = m_texture.id;
    glBindTexture(GL_TEXTURE_2D, id);

    if (m_hasFillColor) {
            m_source = QVolatileImage(w, h, QImage::Format_ARGB32_Premultiplied);
            m_source.fill(PREMUL(m_fillColor.rgba()));

        m_hasFillColor = false;

        GLenum format = qt_gl_preferredTextureFormat();
        QImage tx(w, h, QImage::Format_ARGB32_Premultiplied);
        tx.fill(qt_gl_convertToGLFormat(m_fillColor.rgba(), format));
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, format, GL_UNSIGNED_BYTE, tx.constBits());
    }

    return id;
}

QGLTexture* QGLPixmapData::texture() const
{
    return &m_texture;
}

Q_GUI_EXPORT int qt_defaultDpiX();
Q_GUI_EXPORT int qt_defaultDpiY();

int QGLPixmapData::metric(QPaintDevice::PaintDeviceMetric metric) const
{
    if (w == 0)
        return 0;

    switch (metric) {
    case QPaintDevice::PdmWidth:
        return w;
    case QPaintDevice::PdmHeight:
        return h;
    case QPaintDevice::PdmNumColors:
        return 0;
    case QPaintDevice::PdmDepth:
        return d;
    case QPaintDevice::PdmWidthMM:
        return qRound(w * 25.4 / qt_defaultDpiX());
    case QPaintDevice::PdmHeightMM:
        return qRound(h * 25.4 / qt_defaultDpiY());
    case QPaintDevice::PdmDpiX:
    case QPaintDevice::PdmPhysicalDpiX:
        return qt_defaultDpiX();
    case QPaintDevice::PdmDpiY:
    case QPaintDevice::PdmPhysicalDpiY:
        return qt_defaultDpiY();
    default:
        qWarning("QGLPixmapData::metric(): Invalid metric");
        return 0;
    }
}

// Force the pixmap data to be backed by some valid data.
void QGLPixmapData::forceToImage()
{
    if (!isValid())
        return;

    if (m_source.isNull()) {
        QImage::Format format = QImage::Format_ARGB32_Premultiplied;
        if (pixelType() == BitmapType)
            format = QImage::Format_MonoLSB;
        m_source = QVolatileImage(w, h, format);
    }

    m_dirty = true;
}

void QGLPixmapData::destroyTexture()
{
    if (m_texture.id) {
        QGLWidget *shareWidget = qt_gl_share_widget();
        if (shareWidget) {
            m_texture.options |= QGLContext::MemoryManagedBindOption;
            m_texture.freeTexture();
            m_texture.options &= ~QGLContext::MemoryManagedBindOption;
        } else if(QGLContext::currentContext()) {
            glDeleteTextures(1, &m_texture.id);
            m_texture.id = 0;
            m_texture.boundPixmap = 0;
            m_texture.boundKey = 0;
        }
        m_ctx = 0;
        m_dirty = true;
    }
}

void QGLPixmapData::detachTextureFromPool()
{
    QGLTexturePool::instance()->detachTexture(&m_texture);
}

void QGLPixmapData::hibernate()
{
    destroyTexture();
}

void QGLPixmapData::reclaimTexture()
{
    if (!m_texture.inTexturePool)
        return;

    forceToImage();

    destroyTexture();
}

QGLPaintDevice *QGLPixmapData::glDevice() const
{
    return 0;
}

static inline bool knownGoodFormat(QImage::Format format)
{
    switch (format) {
        case QImage::Format_RGB16: // EColor64K
        case QImage::Format_RGB32: // EColor16MU
        case QImage::Format_ARGB32_Premultiplied: // EColor16MAP
            return true;
        default:
            return false;
    }
}

#ifdef QT_SYMBIAN_SUPPORTS_SGIMAGE
static inline int symbianPixeFormatBitsPerPixel(TUidPixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case EUidPixelFormatP_1:
        case EUidPixelFormatL_1:
            return 1;
        case EUidPixelFormatP_2:
        case EUidPixelFormatL_2:
            return 2;
        case EUidPixelFormatP_4:
        case EUidPixelFormatL_4:
            return 4;
        case EUidPixelFormatRGB_332:
        case EUidPixelFormatA_8:
        case EUidPixelFormatBGR_332:
        case EUidPixelFormatP_8:
        case EUidPixelFormatL_8:
            return 8;
        case EUidPixelFormatRGB_565:
        case EUidPixelFormatBGR_565:
        case EUidPixelFormatARGB_1555:
        case EUidPixelFormatXRGB_1555:
        case EUidPixelFormatARGB_4444:
        case EUidPixelFormatARGB_8332:
        case EUidPixelFormatBGRX_5551:
        case EUidPixelFormatBGRA_5551:
        case EUidPixelFormatBGRA_4444:
        case EUidPixelFormatBGRX_4444:
        case EUidPixelFormatAP_88:
        case EUidPixelFormatXRGB_4444:
        case EUidPixelFormatXBGR_4444:
            return 16;
        case EUidPixelFormatBGR_888:
        case EUidPixelFormatRGB_888:
            return 24;
        case EUidPixelFormatXRGB_8888:
        case EUidPixelFormatBGRX_8888:
        case EUidPixelFormatXBGR_8888:
        case EUidPixelFormatBGRA_8888:
        case EUidPixelFormatARGB_8888:
        case EUidPixelFormatABGR_8888:
        case EUidPixelFormatARGB_8888_PRE:
        case EUidPixelFormatABGR_8888_PRE:
        case EUidPixelFormatBGRA_8888_PRE:
        case EUidPixelFormatARGB_2101010:
        case EUidPixelFormatABGR_2101010:
            return 32;
        default:
            return 32;
    };
}
#endif

void QGLPixmapData::fromNativeType(void* pixmap, NativeType type)
{
    if (type == QPixmapData::SgImage && pixmap) {
#if defined(QT_SYMBIAN_SUPPORTS_SGIMAGE) && !defined(QT_NO_EGL)
        RSgImage *sgImage = reinterpret_cast<RSgImage*>(pixmap);

        m_sgImage = new RSgImage;
        m_sgImage->Open(sgImage->Id());

        TSgImageInfo info;
        sgImage->GetInfo(info);

        w = info.iSizeInPixels.iWidth;
        h = info.iSizeInPixels.iHeight;
        d = symbianPixeFormatBitsPerPixel((TUidPixelFormat)info.iPixelFormat);

        m_source = QVolatileImage();
        m_hasAlpha = true;
        m_hasFillColor = false;
        m_dirty = true;
        is_null = (w <= 0 || h <= 0);
#endif
    } else if (type == QPixmapData::FbsBitmap && pixmap) {
        CFbsBitmap *bitmap = reinterpret_cast<CFbsBitmap *>(pixmap);
        QSize size(bitmap->SizeInPixels().iWidth, bitmap->SizeInPixels().iHeight);
        if (size.width() == w && size.height() == h)
            setSerialNumber(++qt_gl_pixmap_serial);
        resize(size.width(), size.height());
        m_source = QVolatileImage(bitmap);
        if (pixelType() == BitmapType) {
            m_source.ensureFormat(QImage::Format_MonoLSB);
        } else if (!knownGoodFormat(m_source.format())) {
            m_source.beginDataAccess();
            QImage::Format format = idealFormat(m_source.imageRef(), Qt::AutoColor);
            m_source.endDataAccess(true);
            m_source.ensureFormat(format);
        }
        m_hasAlpha = m_source.hasAlphaChannel();
        m_hasFillColor = false;
        m_dirty = true;
        d = m_source.depth();
    } else if (type == QPixmapData::VolatileImage && pixmap) {
        // Support QS60Style in more efficient skin graphics retrieval.
        QVolatileImage *img = static_cast<QVolatileImage *>(pixmap);
        if (img->width() == w && img->height() == h)
            setSerialNumber(++qt_gl_pixmap_serial);
        resize(img->width(), img->height());
        m_source = *img;
        m_hasAlpha = m_source.hasAlphaChannel();
        m_hasFillColor = false;
        m_dirty = true;
        d = m_source.depth();
    } else if (type == QPixmapData::NativeImageHandleProvider && pixmap) {
        destroyTexture();
        nativeImageHandleProvider = static_cast<QNativeImageHandleProvider *>(pixmap);
        // Cannot defer the retrieval, we need at least the size right away.
        createFromNativeImageHandleProvider();
    }
}

void* QGLPixmapData::toNativeType(NativeType type)
{
    if (type == QPixmapData::FbsBitmap) {
        if (m_source.isNull())
            m_source = QVolatileImage(w, h, QImage::Format_ARGB32_Premultiplied);
        return m_source.duplicateNativeImage();
    }

    return 0;
}

bool QGLPixmapData::initFromNativeImageHandle(void *handle, const QString &type)
{
    if (type == QLatin1String("RSgImage")) {
        fromNativeType(handle, QPixmapData::SgImage);
        return true;
    } else if (type == QLatin1String("CFbsBitmap")) {
        fromNativeType(handle, QPixmapData::FbsBitmap);
        return true;
    }
    return false;
}

void QGLPixmapData::createFromNativeImageHandleProvider()
{
    void *handle = 0;
    QString type;
    nativeImageHandleProvider->get(&handle, &type);
    if (handle) {
        if (initFromNativeImageHandle(handle, type)) {
            nativeImageHandle = handle;
            nativeImageType = type;
        } else {
            qWarning("QGLPixmapData: Unknown native image type '%s'", qPrintable(type));
        }
    } else {
        qWarning("QGLPixmapData: Native handle is null");
    }
}

void QGLPixmapData::releaseNativeImageHandle()
{
    if (nativeImageHandleProvider && nativeImageHandle) {
        nativeImageHandleProvider->release(nativeImageHandle, nativeImageType);
        nativeImageHandle = 0;
        nativeImageType = QString();
    }
}

QT_END_NAMESPACE
