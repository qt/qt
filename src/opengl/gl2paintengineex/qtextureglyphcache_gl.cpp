/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qtextureglyphcache_gl_p.h"
#include "qpaintengineex_opengl2_p.h"

#if defined QT_OPENGL_ES_2 && !defined(QT_NO_EGL)
#include "private/qeglcontext_p.h"
#endif

QT_BEGIN_NAMESPACE

#ifdef Q_WS_WIN
extern Q_GUI_EXPORT bool qt_cleartype_enabled;
#endif

QGLTextureGlyphCache::QGLTextureGlyphCache(QGLContext *context, QFontEngineGlyphCache::Type type, const QTransform &matrix)
    : QImageTextureGlyphCache(type, matrix)
    , ctx(context)
    , m_width(0)
    , m_height(0)
{
    // broken FBO readback is a bug in the SGX 1.3 and 1.4 drivers for the N900 where
    // copying between FBO's is broken if the texture is either GL_ALPHA or POT. The
    // workaround is to use a system-memory copy of the glyph cache for this device.
    // Switching to NPOT and GL_RGBA would both cost a lot more graphics memory and
    // be slower, so that is not desireable.
    if (!ctx->d_ptr->workaround_brokenFBOReadBack)
        glGenFramebuffers(1, &m_fbo);

    connect(QGLSignalProxy::instance(), SIGNAL(aboutToDestroyContext(const QGLContext*)),
            SLOT(contextDestroyed(const QGLContext*)));
}

QGLTextureGlyphCache::~QGLTextureGlyphCache()
{
    if (ctx) {
        QGLShareContextScope scope(ctx);

        if (!ctx->d_ptr->workaround_brokenFBOReadBack)
            glDeleteFramebuffers(1, &m_fbo);

        if (m_width || m_height)
            glDeleteTextures(1, &m_texture);
    }
}

void QGLTextureGlyphCache::createTextureData(int width, int height)
{
    // create in QImageTextureGlyphCache baseclass is meant to be called
    // only to create the initial image and does not preserve the content,
    // so we don't call when this function is called from resize.
    if (ctx->d_ptr->workaround_brokenFBOReadBack && image().isNull())
        QImageTextureGlyphCache::createTextureData(width, height);

    // Make the lower glyph texture size 16 x 16.
    if (width < 16)
        width = 16;
    if (height < 16)
        height = 16;

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    m_width = width;
    m_height = height;

    QVarLengthArray<uchar> data(width * height);
    for (int i = 0; i < data.size(); ++i)
        data[i] = 0;

    if (m_type == QFontEngineGlyphCache::Raster_RGBMask)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, &data[0]);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, &data[0]);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void QGLTextureGlyphCache::resizeTextureData(int width, int height)
{
    int oldWidth = m_width;
    int oldHeight = m_height;

    // Make the lower glyph texture size 16 x 16.
    if (width < 16)
        width = 16;
    if (height < 16)
        height = 16;

    GLuint oldTexture = m_texture;
    createTextureData(width, height);
    
    if (ctx->d_ptr->workaround_brokenFBOReadBack) {
        QImageTextureGlyphCache::resizeTextureData(width, height);
        Q_ASSERT(image().depth() == 8);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, oldWidth, oldHeight, GL_ALPHA, GL_UNSIGNED_BYTE, image().constBits());
        glDeleteTextures(1, &oldTexture);
        return;
    }

    // ### the QTextureGlyphCache API needs to be reworked to allow
    // ### resizeTextureData to fail

    glBindFramebuffer(GL_FRAMEBUFFER_EXT, m_fbo);

    GLuint tmp_texture;
    glGenTextures(1, &tmp_texture);
    glBindTexture(GL_TEXTURE_2D, tmp_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, oldWidth, oldHeight, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                           GL_TEXTURE_2D, tmp_texture, 0);

    glActiveTexture(GL_TEXTURE0 + QT_IMAGE_TEXTURE_UNIT);
    glBindTexture(GL_TEXTURE_2D, oldTexture);

    pex->transferMode(BrushDrawingMode);

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_BLEND);

    glViewport(0, 0, oldWidth, oldHeight);

    GLfloat* vertexCoordinateArray = pex->staticVertexCoordinateArray;
    vertexCoordinateArray[0] = -1.0f;
    vertexCoordinateArray[1] = -1.0f;
    vertexCoordinateArray[2] =  1.0f;
    vertexCoordinateArray[3] = -1.0f;
    vertexCoordinateArray[4] =  1.0f;
    vertexCoordinateArray[5] =  1.0f;
    vertexCoordinateArray[6] = -1.0f;
    vertexCoordinateArray[7] =  1.0f;

    GLfloat* textureCoordinateArray = pex->staticTextureCoordinateArray;
    textureCoordinateArray[0] = 0.0f;
    textureCoordinateArray[1] = 0.0f;
    textureCoordinateArray[2] = 1.0f;
    textureCoordinateArray[3] = 0.0f;
    textureCoordinateArray[4] = 1.0f;
    textureCoordinateArray[5] = 1.0f;
    textureCoordinateArray[6] = 0.0f;
    textureCoordinateArray[7] = 1.0f;

    pex->setVertexAttributePointer(QT_VERTEX_COORDS_ATTR, vertexCoordinateArray);
    pex->setVertexAttributePointer(QT_TEXTURE_COORDS_ATTR, textureCoordinateArray);

    pex->shaderManager->useBlitProgram();
    pex->shaderManager->blitProgram()->setUniformValue("imageTexture", QT_IMAGE_TEXTURE_UNIT);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindTexture(GL_TEXTURE_2D, m_texture);

    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, oldWidth, oldHeight);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                              GL_RENDERBUFFER_EXT, 0);
    glDeleteTextures(1, &tmp_texture);
    glDeleteTextures(1, &oldTexture);

    glBindFramebuffer(GL_FRAMEBUFFER_EXT, ctx->d_ptr->current_fbo);

    glViewport(0, 0, pex->width, pex->height);
    pex->updateClipScissorTest();
}

void QGLTextureGlyphCache::fillTexture(const Coord &c, glyph_t glyph)
{
    if (ctx->d_ptr->workaround_brokenFBOReadBack) {
        QImageTextureGlyphCache::fillTexture(c, glyph);

        glBindTexture(GL_TEXTURE_2D, m_texture);
        const QImage &texture = image();
        const uchar *bits = texture.constBits();
        bits += c.y * texture.bytesPerLine() + c.x;
        for (int i=0; i<c.h; ++i) {
            glTexSubImage2D(GL_TEXTURE_2D, 0, c.x, c.y + i, c.w, 1, GL_ALPHA, GL_UNSIGNED_BYTE, bits);
            bits += texture.bytesPerLine();
        }

        return;
    }

    QImage mask = textureMapForGlyph(glyph);
    const int maskWidth = mask.width();
    const int maskHeight = mask.height();

    if (mask.format() == QImage::Format_Mono) {
        mask = mask.convertToFormat(QImage::Format_Indexed8);
        for (int y = 0; y < maskHeight; ++y) {
            uchar *src = (uchar *) mask.scanLine(y);
            for (int x = 0; x < maskWidth; ++x)
                src[x] = -src[x]; // convert 0 and 1 into 0 and 255
        }
    } else if (mask.format() == QImage::Format_RGB32) {
        // Make the alpha component equal to the average of the RGB values.
        // This is needed when drawing sub-pixel antialiased text on translucent targets.
        for (int y = 0; y < maskHeight; ++y) {
            quint32 *src = (quint32 *) mask.scanLine(y);
            for (int x = 0; x < maskWidth; ++x) {
                uchar r = src[x] >> 16;
                uchar g = src[x] >> 8;
                uchar b = src[x];
                quint32 avg = (quint32(r) + quint32(g) + quint32(b) + 1) / 3; // "+1" for rounding.
                src[x] = (src[x] & 0x00ffffff) | (avg << 24);
            }
        }
    }

    glBindTexture(GL_TEXTURE_2D, m_texture);
    if (mask.format() == QImage::Format_RGB32) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, c.x, c.y, maskWidth, maskHeight, GL_BGRA, GL_UNSIGNED_BYTE, mask.bits());
    } else {
#ifdef QT_OPENGL_ES2
        glTexSubImage2D(GL_TEXTURE_2D, 0, c.x, c.y, maskWidth, maskHeight, GL_ALPHA, GL_UNSIGNED_BYTE, mask.bits());
#else
        // glTexSubImage2D() might cause some garbage to appear in the texture if the mask width is
        // not a multiple of four bytes. The bug appeared on a computer with 32-bit Windows Vista
        // and nVidia GeForce 8500GT. GL_UNPACK_ALIGNMENT is set to four bytes, 'mask' has a
        // multiple of four bytes per line, and most of the glyph shows up correctly in the
        // texture, which makes me think that this is a driver bug.
        // One workaround is to make sure the mask width is a multiple of four bytes, for instance
        // by converting it to a format with four bytes per pixel. Another is to copy one line at a
        // time.

        for (int i = 0; i < maskHeight; ++i)
            glTexSubImage2D(GL_TEXTURE_2D, 0, c.x, c.y + i, maskWidth, 1, GL_ALPHA, GL_UNSIGNED_BYTE, mask.scanLine(i));
#endif
    }
}

int QGLTextureGlyphCache::glyphPadding() const
{
    return 1;
}

QT_END_NAMESPACE
