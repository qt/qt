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
    \qmlclass BlendedImage
    \brief The BlendedImage elements blends two different images depending on a blend ratio.

    This element can be used to simulate blur on slow devices by setting secondaryUrl with
    a pre-rendered blurred version of primaryUrl.

    Note that this class will only work under OpenGL. On the software canvas it will display
    only the primary image unless the blend is > 0.75, in which case it will display only the
    secondary image.
*/

/*!
    \internal
    \class QFxBlendedImage
    \brief The QFxBlendedImage blends two different images depending on a blend ratio.

    This class can be used to simulate blur on slow devices by setting secondaryUrl with
    a pre-rendered blurred version of primaryUrl.

    Note that this class will only work under OpenGL. On the software canvas it will display
    only the primary image unless the blend is > 0.75, in which case it will display only the
    secondary image.
*/
QFxBlendedImage::QFxBlendedImage(QFxItem *parent)
: QFxItem(parent), _blend(0), _smooth(false), dirty(false)
{
#if defined(QFX_RENDER_OPENGL2)
    setOptions(HasContents);
#endif
}

/*!
    Cancels any pending image loads and destroys the image.
*/
QFxBlendedImage::~QFxBlendedImage()
{
    if (!primUrl.isEmpty())
        QFxPixmap::cancelGet(primUrl,this);
    if (!secUrl.isEmpty())
        QFxPixmap::cancelGet(secUrl,this);
}

/*!
    \qmlproperty string BlendedImage::primaryUrl
    The URL of the first image to be displayed in this item.
*/
QUrl QFxBlendedImage::primaryUrl() const
{
    return primUrl;
}

void QFxBlendedImage::primaryLoaded()
{
    primPix = QFxPixmap(primUrl);
    dirty = true;
    update();
}

void QFxBlendedImage::setPrimaryUrl(const QUrl &url)
{
    if (primUrl == url)
        return;
    if (!primUrl.isEmpty())
        QFxPixmap::cancelGet(primUrl,this);
    Q_ASSERT(!url.isRelative());
    primUrl = url;
    if (!primUrl.isEmpty())
        QFxPixmap::get(qmlEngine(this), primUrl,this,SLOT(primaryLoaded()));
}

/*!
    \qmlproperty string BlendedImage::secondaryUrl
    The URL of the second image to be displayed in this item.
*/
QUrl QFxBlendedImage::secondaryUrl() const
{
    return secUrl;
}

void QFxBlendedImage::secondaryLoaded()
{
    secPix = QFxPixmap(secUrl);
    dirty = true;
    update();
}

void QFxBlendedImage::setSecondaryUrl(const QUrl &url)
{
    if (secUrl == url)
        return;
    if (!secUrl.isEmpty())
        QFxPixmap::cancelGet(secUrl,this);
    Q_ASSERT(!url.isRelative());
    secUrl = url;
    if (!secUrl.isEmpty())
        QFxPixmap::get(qmlEngine(this), secUrl,this,SLOT(secondaryLoaded()));
}

/*!
    \qmlproperty real BlendedImage::blend
    The ratio used to blend the two images.

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

/*!
    \qmlproperty bool BlendedImage::smooth

    Set this property if you want the image to be smoothly filtered when scaled or
    transformed.  Smooth filtering gives better visual quality, but is slower.  If
    the BlendedImage is displayed at its natural size, this property has no visual or
    performance effect.

    \note Generally scaling artifacts are only visible if the image is stationary on
    the screen.  A common pattern when animating an image is to disable smooth
    filtering at the beginning of the animation and reenable it at the conclusion.
 */
bool QFxBlendedImage::smoothTransform() const
{
    return _smooth;
}

void QFxBlendedImage::setSmoothTransform(bool s)
{
    if (_smooth == s)
        return;
    _smooth = s;
    update();
}

#if defined(QFX_RENDER_QPAINTER) 

void QFxBlendedImage::paintContents(QPainter &p)
{
    if (primUrl.isEmpty() && secUrl.isEmpty())
        return;

    if (_smooth) {
        p.save();
        p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, _smooth);
    }

    if (_blend < 0.75)
        p.drawPixmap(0, 0, primPix);
    else
        p.drawPixmap(0, 0, secPix);

    if (_smooth) {
        p.restore();
    }
}

#elif defined(QFX_RENDER_OPENGL2)

void QFxBlendedImage::paintGLContents(GLPainter &p)
{
    static DualTextureBlendShader *shader = 0;
    if (!shader)
        shader = new DualTextureBlendShader();

    if (dirty) {
        prim.clear();
        sec.clear();
        prim.setImage(primPix.toImage());
        sec.setImage(secPix.toImage());

        dirty = false;
    }

    if (prim.isNull() || sec.isNull()) {

        return;
    }

    GLfloat vertices[8];
    GLfloat texVertices[8];

    float widthV = width();
    float heightV = height();
    if (!widthV)
        widthV = qMax(primPix.width(), secPix.width());
    if (!heightV)
        heightV = qMax(primPix.height(), secPix.height());

    vertices[0] = 0; vertices[1] = heightV;
    vertices[2] = widthV; vertices[3] = heightV;
    vertices[4] = 0; vertices[5] = 0;
    vertices[6] = widthV; vertices[7] = 0;

    texVertices[0] = 0; texVertices[1] = 0;
    texVertices[2] = 1; texVertices[3] = 0;
    texVertices[4] = 0; texVertices[5] = 1;
    texVertices[6] = 1; texVertices[7] = 1;

    if (_blend == 0 || _blend == 1) {
        QGLShaderProgram *tshader = p.useTextureShader();

        GLTexture *tex = 0;

        if (_blend == 0)
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
        if (b > 1) b = 1;
        else if (b < 0) b = 0;
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

QML_DEFINE_TYPE(QFxBlendedImage,BlendedImage)

QT_END_NAMESPACE
