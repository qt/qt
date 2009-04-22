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

#include "qfxblendedimage.h"
#include <QtDeclarative/qmlcontext.h>

#if defined(QFX_RENDER_OPENGL2)
#include <glbasicshaders.h>
#endif

QT_BEGIN_NAMESPACE

/*!
    \internal
    \class QFxBlendedImage
    \brief The QFxBlendedImage blends two different images depending on a blend ratio.

    This class can be used to simulate blur on slow devices by setting secondaryFile with
    a pre-rendered blurred version of primaryFile.

    Note that this class will only work under OpenGL. On the software canvas it will display
    only the primary image unless the blend is > 0.75, in which case it will display only the
    secondary image.
*/
QFxBlendedImage::QFxBlendedImage(QFxItem *parent)
: QFxItem(parent), _blend(0), dirty(false)
{
#if defined(QFX_RENDER_OPENGL2)
    setOptions(HasContents);
#endif
}

/*!
    \property QFxBlendedImage::primaryUrl
    \brief the URL of the first image to be displayed in this item.
*/
QString QFxBlendedImage::primaryUrl() const
{
    return primSrc;
}

void QFxBlendedImage::primaryLoaded()
{
    primPix = QFxPixmap(primUrl);
    dirty = true;
    update();
}

void QFxBlendedImage::setPrimaryUrl(const QString &url)
{
    if (primSrc == url)
        return;
    if (!primSrc.isEmpty())
        QFxPixmap::cancelGet(primUrl,this,SLOT(primaryLoaded()));
    primSrc = url;
    primUrl = itemContext()->resolvedUrl(url);
    if (!primSrc.isEmpty())
        QFxPixmap::get(itemContext()->engine(), primUrl,this,SLOT(primaryLoaded()));
}

/*!
    \property QFxBlendedImage::secondaryFile
    \brief the URL of the second image to be displayed in this item.
*/
QString QFxBlendedImage::secondaryUrl() const
{
    return secSrc;
}

void QFxBlendedImage::secondaryLoaded()
{
    secPix = QFxPixmap(secUrl);
    dirty = true;
    update();
}

void QFxBlendedImage::setSecondaryUrl(const QString &url)
{
    if (secSrc == url)
        return;
    if (!secSrc.isEmpty())
        QFxPixmap::cancelGet(secUrl,this,SLOT(secondaryLoaded()));
    secSrc = url;
    secUrl = itemContext()->resolvedUrl(url);
    if (!secSrc.isEmpty())
        QFxPixmap::get(itemContext()->engine(), secUrl,this,SLOT(secondaryLoaded()));
}

/*!
    \property QFxBlendedImage::blend
    \brief the ratio used to blend the two images.

    If blend has a value of 0, only the first image will be displayed.
    If blend has a value of 1, only the second image will be displayed.
*/
qreal QFxBlendedImage::blend() const
{
    return _blend;
}

void QFxBlendedImage::setBlend(qreal b)
{
    _blend = b;
    update();
}

#if defined(QFX_RENDER_QPAINTER) 

void QFxBlendedImage::paintContents(QPainter &p)
{
    if (primSrc.isNull() && secSrc.isNull())
        return;
    if (_blend < 0.75)
        p.drawImage(0, 0, primPix);
    else
        p.drawImage(0, 0, secPix);
}

#elif defined(QFX_RENDER_OPENGL2)

void QFxBlendedImage::paintGLContents(GLPainter &p)
{
    static DualTextureBlendShader *shader = 0;
    if(!shader)
        shader = new DualTextureBlendShader();

    if(dirty) {
        prim.clear();
        sec.clear();
        prim.setImage(primPix);
        sec.setImage(secPix);

        dirty = false;
    }

    if(prim.isNull() || sec.isNull()) {

        return;
    }

    GLfloat vertices[8];
    GLfloat texVertices[8];

    float widthV = width();
    float heightV = height();
    if(!widthV)
        widthV = qMax(primPix.width(), secPix.width());
    if(!heightV)
        heightV = qMax(primPix.height(), secPix.height());

    vertices[0] = 0; vertices[1] = heightV;
    vertices[2] = widthV; vertices[3] = heightV;
    vertices[4] = 0; vertices[5] = 0;
    vertices[6] = widthV; vertices[7] = 0;

    texVertices[0] = 0; texVertices[1] = 0;
    texVertices[2] = 1; texVertices[3] = 0;
    texVertices[4] = 0; texVertices[5] = 1;
    texVertices[6] = 1; texVertices[7] = 1;

    if(_blend == 0 || _blend == 1) {
        QGLShaderProgram *tshader = p.useTextureShader();

        GLTexture *tex = 0;

        if(_blend == 0)
            tex = &prim;
        else
            tex = &sec;

        tshader->setAttributeArray(SingleTextureShader::Vertices, vertices, 2);
        tshader->setAttributeArray(SingleTextureShader::TextureCoords, texVertices, 2);

        glBindTexture(GL_TEXTURE_2D, tex->texture());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        tshader->disableAttributeArray(SingleTextureShader::Vertices);
        tshader->disableAttributeArray(SingleTextureShader::TextureCoords);
    } else {

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, prim.texture());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, sec.texture());

        shader->enable();
        shader->setOpacity(1);
        qreal b = _blend;
        if(b > 1) b = 1;
        else if(b < 0) b = 0;
        shader->setBlend(b);
        shader->setTransform(p.activeTransform);

        shader->setAttributeArray(DualTextureBlendShader::Vertices, vertices, 2);
        shader->setAttributeArray(DualTextureBlendShader::TextureCoords, texVertices, 2);
        shader->setAttributeArray(DualTextureBlendShader::BlendTextureCoords, texVertices, 2);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        shader->disableAttributeArray(DualTextureBlendShader::Vertices);
        shader->disableAttributeArray(DualTextureBlendShader::TextureCoords);
        shader->disableAttributeArray(DualTextureBlendShader::BlendTextureCoords);

        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
    }
}
#endif

QML_DEFINE_TYPE(QFxBlendedImage,BlendedImage);
QT_END_NAMESPACE
