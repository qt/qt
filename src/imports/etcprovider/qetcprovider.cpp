/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Declarative module of the Qt Toolkit.
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

#include "qetcprovider.h"

#include <QtDebug>
#include <QFile>

#include <qglfunctions.h>

QT_BEGIN_NAMESPACE

typedef struct {
    char aName[6];
    unsigned short iBlank;
    /* NB: Beware endianess issues here. */
    unsigned char iPaddedWidthMSB;
    unsigned char iPaddedWidthLSB;
    unsigned char iPaddedHeightMSB;
    unsigned char iPaddedHeightLSB;
    unsigned char iWidthMSB;
    unsigned char iWidthLSB;
    unsigned char iHeightMSB;
    unsigned char iHeightLSB;
} ETCHeader;


unsigned short getWidth(ETCHeader *pHeader)
{
    return (pHeader->iWidthMSB << 8) | pHeader->iWidthLSB;
}

unsigned short getHeight(ETCHeader *pHeader)
{
    return (pHeader->iHeightMSB << 8) | pHeader->iHeightLSB;
}

unsigned short getPaddedWidth(ETCHeader *pHeader)
{
    return (pHeader->iPaddedWidthMSB << 8) | pHeader->iPaddedWidthLSB;
}

unsigned short getPaddedHeight(ETCHeader *pHeader)
{
    return (pHeader->iPaddedHeightMSB << 8) | pHeader->iPaddedHeightLSB;
}

enum {GL_ETC1_RGB8_OES=0x8d64};

EtcTexture::EtcTexture()
    : m_texture_id(0)
{

}

EtcTexture::~EtcTexture()
{
    if (m_texture_id)
        glDeleteTextures(1, &m_texture_id);
}


void EtcTexture::bind()
{
    if (m_texture_id) {
        glBindTexture(GL_TEXTURE_2D, m_texture_id);
        return;
    }

#ifdef ETC_DEBUG
    printf("EtcTextureProvider: about to update that texture...\n");
#endif

    glGenTextures(1, &m_texture_id);

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

#ifdef ETC_DEBUG
    qDebug() << "glCompressedTexImage2D, width: " << m_size.width() << "height" << m_size.height() <<
                "paddedWidth: " << m_paddedSize.width() << "paddedHeight: " << m_paddedSize.height();
#endif

    const QGLContext *ctx = QGLContext::currentContext();
    Q_ASSERT(ctx != 0);
    ctx->functions()->glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES,
                                             m_size.width(), m_size.height(), 0,
                                             (m_paddedSize.width() * m_paddedSize.height()) >> 1,
                                             m_data.data() + 16);

    // Gracefully fail in case of an error...
    GLuint error = glGetError();
    if (error != GL_NO_ERROR) {
        qDebug () << "glCompressedTexImage2D for compressed texture failed, error: " << error;
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &m_texture_id);
        m_texture_id = 0;
        return;
    }
    updateBindOptions(true);
}

QSize EtcTexture::textureSize() const
{
    return m_size;
}

QSGTexture *QEtcProvider::requestTexture(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(requestedSize);
    EtcTexture *ret = 0;

    size->setHeight(0);
    size->setWidth(0);

    QFile file(id);
#ifdef ETC_DEBUG
    qDebug() << "requestTexture opening file: " << id;
#endif
    if (file.open(QIODevice::ReadOnly)) {
        ret = new EtcTexture();
        ret->m_data = file.readAll();
        if (!ret->m_data.isEmpty()) {
            ETCHeader *pETCHeader = NULL;
            pETCHeader = (ETCHeader *)ret->m_data.data();
            size->setHeight(getHeight(pETCHeader));
            size->setWidth(getWidth(pETCHeader));
            ret->m_size = *size;
            ret->m_paddedSize.setHeight(getPaddedHeight(pETCHeader));
            ret->m_paddedSize.setWidth(getPaddedWidth(pETCHeader));
        }
        else {
            free (ret);
            ret = 0;
        }
    }

#ifdef ETC_DEBUG
    if (ret)
        qDebug() << "requestTexture returning: " << ret->m_data.length() << ", bytes; width: " << size->width() << ", height: " << size->height();
    else
        qDebug () << "File not found.";
#endif

    return ret;
}

QT_END_NAMESPACE
