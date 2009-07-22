/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qpixmap.h"
#include "qglframebufferobject.h"

#include <private/qpaintengine_raster_p.h>

#include "qpixmapdata_gl_p.h"

#include <private/qgl_p.h>
#include <private/qdrawhelper_p.h>

#include <private/qpaintengineex_opengl2_p.h>

QT_BEGIN_NAMESPACE

extern QGLWidget* qt_gl_share_widget();

class QGLShareContextScope
{
public:
    QGLShareContextScope(const QGLContext *ctx)
        : m_oldContext(0)
    {
        QGLContext *currentContext = const_cast<QGLContext *>(QGLContext::currentContext());
        if (currentContext != ctx && !qgl_share_reg()->checkSharing(ctx, currentContext)) {
            m_oldContext = currentContext;
            m_ctx = const_cast<QGLContext *>(ctx);
            m_ctx->makeCurrent();
        } else {
            m_ctx = currentContext;
        }
    }

    operator QGLContext *()
    {
        return m_ctx;
    }

    QGLContext *operator->()
    {
        return m_ctx;
    }

    ~QGLShareContextScope()
    {
        if (m_oldContext)
            m_oldContext->makeCurrent();
    }

private:
    QGLContext *m_oldContext;
    QGLContext *m_ctx;
};

static int qt_gl_pixmap_serial = 0;

QGLPixmapData::QGLPixmapData(PixelType type)
    : QPixmapData(type, OpenGLClass)
    , m_renderFbo(0)
    , m_textureId(0)
    , m_engine(0)
    , m_ctx(0)
    , m_dirty(false)
    , m_hasFillColor(false)
    , m_hasAlpha(false)
{
    setSerialNumber(++qt_gl_pixmap_serial);
}

QGLPixmapData::~QGLPixmapData()
{
    QGLWidget *shareWidget = qt_gl_share_widget();
    if (!shareWidget)
        return;

    if (m_textureId) {
        QGLShareContextScope ctx(shareWidget->context());
        glDeleteTextures(1, &m_textureId);
    }
}

bool QGLPixmapData::isValid() const
{
    return w > 0 && h > 0;
}

bool QGLPixmapData::isValidContext(const QGLContext *ctx) const
{
    if (ctx == m_ctx)
        return true;

    const QGLContext *share_ctx = qt_gl_share_widget()->context();
    return ctx == share_ctx || qgl_share_reg()->checkSharing(ctx, share_ctx);
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

    if (m_textureId) {
        QGLShareContextScope ctx(qt_gl_share_widget()->context());
        glDeleteTextures(1, &m_textureId);
        m_textureId = 0;
    }

    m_source = QImage();
    m_dirty = isValid();
    setSerialNumber(++qt_gl_pixmap_serial);
}

void QGLPixmapData::ensureCreated() const
{
    if (!m_dirty)
        return;

    m_dirty = false;

    QGLShareContextScope ctx(qt_gl_share_widget()->context());
    m_ctx = ctx;

    const GLenum format = qt_gl_preferredTextureFormat();
    const GLenum target = GL_TEXTURE_2D;

    if (!m_textureId) {
        glGenTextures(1, &m_textureId);
        glBindTexture(target, m_textureId);
        GLenum format = m_hasAlpha ? GL_RGBA : GL_RGB;
        glTexImage2D(target, 0, format, w, h, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    if (!m_source.isNull()) {
        const QImage tx = ctx->d_func()->convertToGLFormat(m_source, true, format);

        glBindTexture(target, m_textureId);
        glTexSubImage2D(target, 0, 0, 0, w, h, format,
                        GL_UNSIGNED_BYTE, tx.bits());

        if (useFramebufferObjects())
            m_source = QImage();
    }
}

QGLFramebufferObject *QGLPixmapData::fbo() const
{
    return m_renderFbo;
}

void QGLPixmapData::fromImage(const QImage &image,
                              Qt::ImageConversionFlags)
{
    if (image.size() == QSize(w, h))
        setSerialNumber(++qt_gl_pixmap_serial);
    resize(image.width(), image.height());

    if (pixelType() == BitmapType) {
        m_source = image.convertToFormat(QImage::Format_MonoLSB);
    } else {
        m_source = image.hasAlphaChannel()
            ? image.convertToFormat(QImage::Format_ARGB32_Premultiplied)
            : image.convertToFormat(QImage::Format_RGB32);
    }

    m_dirty = true;
    m_hasFillColor = false;

    m_hasAlpha = image.hasAlphaChannel();
    w = image.width();
    h = image.height();
    is_null = (w <= 0 || h <= 0);
    d = pixelType() == QPixmapData::PixmapType ? 32 : 1;

    if (m_textureId) {
        QGLShareContextScope ctx(qt_gl_share_widget()->context());
        glDeleteTextures(1, &m_textureId);
        m_textureId = 0;
    }
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
    if (data->classId() != QPixmapData::OpenGLClass) {
        QPixmapData::copy(data, rect);
        return;
    }

    // can be optimized to do a framebuffer blit or similar ...
    QPixmapData::copy(data, rect);
}

void QGLPixmapData::fill(const QColor &color)
{
    if (!isValid())
        return;

    bool hasAlpha = color.alpha() != 255;
    if (hasAlpha && !m_hasAlpha) {
        if (m_textureId) {
            glDeleteTextures(1, &m_textureId);
            m_textureId = 0;
            m_dirty = true;
        }
        m_hasAlpha = color.alpha() != 255;
    }

    if (useFramebufferObjects()) {
        m_source = QImage();
        m_hasFillColor = true;
        m_fillColor = color;
    } else {
        QImage image = fillImage(color);
        fromImage(image, 0);
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
        img.setNumColors(2);
        img.setColor(0, QColor(Qt::color0).rgba());
        img.setColor(1, QColor(Qt::color1).rgba());

        int gray = qGray(color.rgba());
        if (qAbs(255 - gray) < gray)
            img.fill(0);
        else
            img.fill(1);
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

    if (m_renderFbo) {
        copyBackFromRenderFbo(true);
    } else if (!m_source.isNull()) {
        return m_source;
    } else if (m_dirty || m_hasFillColor) {
        return fillImage(m_fillColor);
    } else {
        ensureCreated();
    }

    QGLShareContextScope ctx(qt_gl_share_widget()->context());
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    return qt_gl_read_texture(QSize(w, h), true, true);
}

struct TextureBuffer
{
    QGLFramebufferObject *fbo;
    QGL2PaintEngineEx *engine;
};

static QVector<TextureBuffer> textureBufferStack;
static int currentTextureBuffer = 0;

void QGLPixmapData::copyBackFromRenderFbo(bool keepCurrentFboBound) const
{
    if (!isValid())
        return;

    m_hasFillColor = false;

    const QGLContext *share_ctx = qt_gl_share_widget()->context();
    QGLShareContextScope ctx(share_ctx);

    ensureCreated();

    if (!ctx->d_ptr->fbo)
        glGenFramebuffers(1, &ctx->d_ptr->fbo);

    glBindFramebuffer(GL_FRAMEBUFFER_EXT, ctx->d_ptr->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
        GL_TEXTURE_2D, m_textureId, 0);

    const int x0 = 0;
    const int x1 = w;
    const int y0 = 0;
    const int y1 = h;

    glBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, m_renderFbo->handle());

    glDisable(GL_SCISSOR_TEST);

    glBlitFramebufferEXT(x0, y0, x1, y1,
            x0, y0, x1, y1,
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST);

    if (keepCurrentFboBound)
        glBindFramebuffer(GL_FRAMEBUFFER_EXT, ctx->d_ptr->current_fbo);
}

void QGLPixmapData::swapBuffers()
{
    if (!isValid())
        return;

    copyBackFromRenderFbo(false);
    m_renderFbo->release();

    --currentTextureBuffer;

    m_renderFbo = 0;
    m_engine = 0;
}

void QGLPixmapData::makeCurrent()
{
    if (isValid() && m_renderFbo)
        m_renderFbo->bind();
}

void QGLPixmapData::doneCurrent()
{
    if (isValid() && m_renderFbo)
        m_renderFbo->release();
}

static TextureBuffer createTextureBuffer(const QSize &size, QGL2PaintEngineEx *engine = 0)
{
    TextureBuffer buffer;
    QGLFramebufferObjectFormat fmt;
    fmt.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
    fmt.setSamples(4);

    buffer.fbo = new QGLFramebufferObject(size, fmt);
    buffer.engine = engine ? engine : new QGL2PaintEngineEx;

    return buffer;
}

bool QGLPixmapData::useFramebufferObjects()
{
    return QGLFramebufferObject::hasOpenGLFramebufferObjects()
           && QGLFramebufferObject::hasOpenGLFramebufferBlit()
           && qt_gl_preferGL2Engine();
}

QPaintEngine* QGLPixmapData::paintEngine() const
{
    if (!isValid())
        return 0;

    if (m_engine)
        return m_engine;

    if (useFramebufferObjects()) {
        extern QGLWidget* qt_gl_share_widget();

        if (!QGLContext::currentContext())
            qt_gl_share_widget()->makeCurrent();
        QGLShareContextScope ctx(qt_gl_share_widget()->context());

        if (textureBufferStack.size() <= currentTextureBuffer) {
            textureBufferStack << createTextureBuffer(size());
        } else {
            QSize sz = textureBufferStack.at(currentTextureBuffer).fbo->size();
            if (sz.width() < w || sz.height() < h) {
                if (sz.width() < w)
                    sz.setWidth(qMax(w, qRound(sz.width() * 1.5)));
                if (sz.height() < h)
                    sz.setHeight(qMax(h, qRound(sz.height() * 1.5)));

                // wasting too much space?
                if (sz.width() * sz.height() > w * h * 2.5)
                    sz = QSize(w, h);

                delete textureBufferStack.at(currentTextureBuffer).fbo;
                textureBufferStack[currentTextureBuffer] =
                    createTextureBuffer(sz, textureBufferStack.at(currentTextureBuffer).engine);
                qDebug() << "Creating new pixmap texture buffer:" << sz;
            }
        }

        if (textureBufferStack.at(currentTextureBuffer).fbo->isValid()) {
            m_renderFbo = textureBufferStack.at(currentTextureBuffer).fbo;
            m_engine = textureBufferStack.at(currentTextureBuffer).engine;

            ++currentTextureBuffer;

            return m_engine;
        }

        qWarning() << "Failed to create pixmap texture buffer of size " << size() << ", falling back to raster paint engine";
    }

    m_dirty = true;
    if (m_source.size() != size())
        m_source = QImage(size(), QImage::Format_ARGB32_Premultiplied);
    if (m_hasFillColor) {
        m_source.fill(PREMUL(m_fillColor.rgba()));
        m_hasFillColor = false;
    }
    return m_source.paintEngine();
}

GLuint QGLPixmapData::bind(bool copyBack) const
{
    if (m_renderFbo && copyBack) {
        copyBackFromRenderFbo(true);
    } else {
        if (m_hasFillColor) {
            m_dirty = true;
            m_source = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
            m_source.fill(PREMUL(m_fillColor.rgba()));
            m_hasFillColor = false;
        }
        ensureCreated();
    }

    GLuint id = m_textureId;
    glBindTexture(GL_TEXTURE_2D, id);
    return id;
}

GLuint QGLPixmapData::textureId() const
{
    ensureCreated();
    return m_textureId;
}

extern int qt_defaultDpiX();
extern int qt_defaultDpiY();

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

QT_END_NAMESPACE
