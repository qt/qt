
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



#include "default_textureatlas.h"

#include "qgltexture2d_p.h"

QT_BEGIN_NAMESPACE

#ifndef GL_UNSIGNED_SHORT_5_6_5
#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#endif

DefaultTextureAtlas::DefaultTextureAtlas(const QSize &size, uint flags)
    : TextureAtlasInterface(flags)
    , m_texture(new QGLTexture2D)
    , m_allocator(size)
{
    QImage image(size, QImage::Format_ARGB32_Premultiplied);
    image.fill(0);
    m_texture->setImage(image);

    m_texture->setHorizontalWrap(QSG::ClampToEdge);
    m_texture->setVerticalWrap(QSG::ClampToEdge);
    m_texture->setBindOptions(QGLContext::InternalBindOption);
}

QGLTexture2DConstPtr DefaultTextureAtlas::texture()
{
    return m_texture.constCast<const QGLTexture2D>();
}

QRect DefaultTextureAtlas::allocate(const QImage &image, bool clampToEdge)
{
    QSize size(image.width() + 2, image.height() + 2);
    QRect allocated = m_allocator.allocate(size);
    if (allocated.isNull())
        return allocated;

    QGLTexture2DPrivate *d = QGLTexture2DPrivate::get(m_texture.data());
    QPainter p(&d->image);
    p.setCompositionMode(QPainter::CompositionMode_Source);

    int dstLoX = allocated.x();
    int dstLoY = allocated.y();
    int dstHiX = allocated.x() + allocated.width() - 1;
    int dstHiY = allocated.y() + allocated.height() - 1;
    int srcLoX = (clampToEdge ? 0 : image.width() - 1);
    int srcLoY = (clampToEdge ? 0 : image.height() - 1);
    int srcHiX = (clampToEdge ? image.width() - 1 : 0);
    int srcHiY = (clampToEdge ? image.height() - 1 : 0);

    p.drawImage(dstLoX + 1, dstLoY + 1, image);

    // Fill margins.
    p.drawImage(QPoint(dstLoX, dstLoY), image, QRect(srcLoX, srcLoY, 1, 1));
    p.drawImage(QPoint(dstLoX, dstHiY), image, QRect(srcLoX, srcHiY, 1, 1));
    p.drawImage(QPoint(dstHiX, dstLoY), image, QRect(srcHiX, srcLoY, 1, 1));
    p.drawImage(QPoint(dstHiX, dstHiY), image, QRect(srcHiX, srcHiY, 1, 1));

    p.drawImage(QPoint(dstLoX + 1, dstLoY), image, QRect(0, srcLoY, image.width(), 1));
    p.drawImage(QPoint(dstLoX, dstLoY + 1), image, QRect(srcLoX, 0, 1, image.height()));
    p.drawImage(QPoint(dstLoX + 1, dstHiY), image, QRect(0, srcHiY, image.width(), 1));
    p.drawImage(QPoint(dstHiX, dstLoY + 1), image, QRect(srcHiX, 0, 1, image.height()));
    p.end();

    if (QGLContext::currentContext() != 0) {
        m_texture->bind();
#ifdef QT_OPENGL_ES_2
        const QImage atlasImage = d->image.copy(allocated);
        if (atlasImage.format() == QImage::Format_RGB16)
            glTexSubImage2D(GL_TEXTURE_2D, 0, allocated.x(), allocated.y(), allocated.width(), allocated.height(), GL_RGB, GL_UNSIGNED_SHORT_5_6_5, atlasImage.constBits());
        else
            glTexSubImage2D(GL_TEXTURE_2D, 0, allocated.x(), allocated.y(), allocated.width(), allocated.height(), GL_BGRA, GL_UNSIGNED_BYTE, atlasImage.constBits());
#else
        const QImage &atlasImage = d->image;
        if (atlasImage.format() == QImage::Format_RGB16) {
            glPixelStorei(GL_UNPACK_ROW_LENGTH, atlasImage.bytesPerLine() >> 1);
            const ushort *p = (const ushort *)atlasImage.scanLine(dstLoY) + dstLoX;
            glTexSubImage2D(GL_TEXTURE_2D, 0, allocated.x(), allocated.y(), allocated.width(), allocated.height(), GL_RGB, GL_UNSIGNED_SHORT_5_6_5, p);
        } else {
            glPixelStorei(GL_UNPACK_ROW_LENGTH, atlasImage.bytesPerLine() >> 2);
            const QRgb *p = (const QRgb *)atlasImage.scanLine(dstLoY) + dstLoX;
            glTexSubImage2D(GL_TEXTURE_2D, 0, allocated.x(), allocated.y(), allocated.width(), allocated.height(), GL_BGRA, GL_UNSIGNED_BYTE, p);
        }
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    } else {
        m_texture->setImage(d->image); // Make sure texture is updated.
    }
    return allocated.adjusted(1, 1, -1, -1);
}

void DefaultTextureAtlas::deallocate(const QRect &rect)
{
    m_allocator.deallocate(rect.adjusted(-1, -1, 1, 1));
}

QT_END_NAMESPACE
