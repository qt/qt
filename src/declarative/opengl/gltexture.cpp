/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "gltexture.h"
#include <QImage>


QT_BEGIN_NAMESPACE
/*!
    \internal
    \class GLTexture
    \brief The GLTexture class simplifies the use of OpenGL textures.
*/

// Copied from QGLWidget::convertToGLFormat
static QImage QGLWidget_convertToGLFormat(const QImage& img)
{
    QImage res = img.convertToFormat(QImage::Format_ARGB32);
    res = res.mirrored();

    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
        // Qt has ARGB; OpenGL wants RGBA
        for (int i=0; i < res.height(); i++) {
            uint *p = (uint*)res.scanLine(i);
            uint *end = p + res.width();
            while (p < end) {
                *p = (*p << 8) | ((*p >> 24) & 0xFF);
                p++;
            }
        }
    }
    else {
        // Qt has ARGB; OpenGL wants ABGR (i.e. RGBA backwards)
        res = res.rgbSwapped();
    }
    return res;
}
class GLTexturePrivate
{
public:
    GLTexturePrivate(GLTexture *_q)
        : q(_q), texture(0), width(0), height(0), 
           horizWrap(GLTexture::Repeat), vertWrap(GLTexture::Repeat),
           minFilter(GLTexture::Linear), magFilter(GLTexture::Linear),
           glWidth(1.), glHeight(1.)
    {
    }

    GLTexture *q;
    GLuint texture;
    int width;
    int height;
    GLTexture::WrapMode horizWrap;
    GLTexture::WrapMode vertWrap;
    GLTexture::FilterMode minFilter;
    GLTexture::FilterMode magFilter;

    qreal glWidth;
    qreal glHeight;
    QSize glSize;

    void genTexture();
};

void GLTexturePrivate::genTexture()
{
    if (texture)
        return;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    q->setHorizontalWrap(horizWrap);
    q->setVerticalWrap(vertWrap);
}

GLTexture::GLTexture()
: d(new GLTexturePrivate(this))
{
}

GLTexture::GLTexture(const QString &file)
: d(new GLTexturePrivate(this))
{
    QImage img(file);
    if (!img.isNull())
        setImage(img);
}

GLTexture::GLTexture(const QImage &img)
: d(new GLTexturePrivate(this))
{
    setImage(img);
}

GLTexture::~GLTexture()
{
    if (d->texture)
        glDeleteTextures(1, &d->texture);
    delete d;
    d = 0;
}

bool GLTexture::isNull() const
{
    return d->texture == 0;
}

void GLTexture::clear()
{
    if (d->texture) {
        glDeleteTextures(1, &d->texture);
        d->texture = 0;
        d->width = 0;
        d->height = 0;
    }
}

static inline int npot(int size)
{    
    size--;
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    size++;
    return size;
}

/*!
    Set the texture to \a img.  If the texture has already been created (either
    by explicitly setting the size, or by previously setting an image), it will
    be destroyed and a new texture created with \a img's contents and size.
 */
void GLTexture::setImage(const QImage &img, ImageMode mode)
{
    if (img.isNull())
        return;

    d->genTexture();

    glBindTexture(GL_TEXTURE_2D, d->texture);

    if (mode == NonPowerOfTwo) {

        if (img.format() == QImage::Format_RGB16) {
            QImage dataImage = img.mirrored();

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dataImage.width(),
                         dataImage.height(), 0, 
                         GL_RGB,
                         GL_UNSIGNED_SHORT_5_6_5, dataImage.bits());
        } else {
            QImage dataImage = QGLWidget_convertToGLFormat(img);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dataImage.width(), 
                         dataImage.height(), 0, 
                         (dataImage.format() == QImage::Format_ARGB32)?GL_RGBA:GL_RGB, 
                         GL_UNSIGNED_BYTE, dataImage.bits());
        }
        d->glWidth = 1.;
        d->glHeight = 1.;
        d->glSize = img.size();

    } else {
        // mode == PowerOfTwo
        int max = (img.width() > img.height())?img.width():img.height();
        max = npot(max);

        if (img.format() == QImage::Format_RGB16) {
            QImage dataImage = img.mirrored();

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, max,
                         max, 0, 
                         GL_RGB,
                         GL_UNSIGNED_SHORT_5_6_5, 0);

            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, dataImage.width(),
                            dataImage.height(), GL_RGB, GL_UNSIGNED_SHORT_5_6_5, 
                            dataImage.bits()); 
            
        } else {
            QImage dataImage = QGLWidget_convertToGLFormat(img);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, max,
                         max, 0, 
                         (dataImage.format() == QImage::Format_ARGB32)?GL_RGBA:GL_RGB, 
                         GL_UNSIGNED_BYTE, 0);

            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, dataImage.width(), 
                         dataImage.height(), 
                         (dataImage.format() == QImage::Format_ARGB32)?GL_RGBA:GL_RGB, 
                         GL_UNSIGNED_BYTE, dataImage.bits());
        }

        d->glWidth = qreal(img.width()) / qreal(max);
        d->glHeight = qreal(img.height()) / qreal(max);
        d->glSize = QSize(max, max);
    }

    d->width = img.width();
    d->height = img.height();
}

void GLTexture::copyImage(const QImage &img, const QPoint &point, 
                          const QRect &srcRect)
{
    qFatal("Not implemented");
    Q_UNUSED(img);
    Q_UNUSED(point);
    Q_UNUSED(srcRect);
}

QSize GLTexture::size() const
{
    return QSize(d->width, d->height);
}

int GLTexture::width() const
{
    return d->width;
}

int GLTexture::height() const
{
    return d->height;
}

qreal GLTexture::glWidth() const
{
    return d->glWidth;
}

qreal GLTexture::glHeight() const
{
    return d->glHeight;
}

QSize GLTexture::glSize() const
{
    return d->glSize;
}

/*!
    Sets the \a size of the texture.  This will destroy the current contents of
    the texture.  If an image has been assigned, it will need to be reassigned
    using either setImage() or copyImage().

    If size is invalid (width or height is less than or equal to 0) the texture
    will be destroyed.  This is equivalent to calling clear().
*/
void GLTexture::setSize(const QSize &size)
{
    if (size.width() <= 0 || size.height() <= 0) {
        clear();
        return;
    }

    d->genTexture();
    glBindTexture(GL_TEXTURE_2D, d->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.width(), size.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    d->width = size.width();
    d->height = size.height();
}

GLTexture::WrapMode GLTexture::horizontalWrap() const
{
    return d->horizWrap;
}

GLTexture::WrapMode GLTexture::verticalWrap() const
{
    return d->vertWrap;
}

void GLTexture::setHorizontalWrap(WrapMode mode)
{
    d->horizWrap = mode; 
    if (d->texture) {
        GLint last;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last);
        if (GLuint(last) != d->texture)
            glBindTexture(GL_TEXTURE_2D, d->texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
        if (GLuint(last) != d->texture)
            glBindTexture(GL_TEXTURE_2D, last);
    }
}

/*!
    Set the veritcal wrap mode to \a mode.
 */
void GLTexture::setVerticalWrap(WrapMode mode)
{
    d->vertWrap = mode; 
    if (d->texture) {
        GLint last;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last);
        if (GLuint(last) != d->texture)
            glBindTexture(GL_TEXTURE_2D, d->texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
        if (GLuint(last) != d->texture)
            glBindTexture(GL_TEXTURE_2D, last);
    }
}

GLTexture::FilterMode GLTexture::minFilter() const
{
    return d->minFilter;
}

GLTexture::FilterMode GLTexture::magFilter() const
{
    return d->magFilter;
}

void GLTexture::setMinFilter(FilterMode f)
{
    if (d->minFilter == f)
       return;
    d->minFilter = f;
    if (d->texture) {
        glBindTexture(GL_TEXTURE_2D, d->texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, d->minFilter);
    }
}

void GLTexture::setMagFilter(FilterMode f)
{
    if (d->magFilter == f)
       return;
    d->magFilter = f;
    if (d->texture) {
        glBindTexture(GL_TEXTURE_2D, d->texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, d->magFilter);
    }
}

GLuint GLTexture::texture() const
{
    return d->texture;
}

QT_END_NAMESPACE
