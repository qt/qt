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

#include "qfximage.h"
#include "qfximage_p.h"
#include <qfxperf.h>
#if defined(QFX_RENDER_OPENGL)
#include <glsave.h>
#endif
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QtDeclarative/qmlengine.h>

QT_BEGIN_NAMESPACE


QML_DEFINE_TYPE(QFxImage,Image);

/*!
    \qmlclass Image QFxImage
    \brief The Image element allows you to add bitmaps to a scene.
    \inherits Item

    The Image element supports untransformed, stretched, grid-scaled and tiled images. For an explanation of grid-scaling see the scaleGrid property description or the QFxScaleGrid class description.

    Examples:
    \table
    \row
    \o \image declarative-qtlogo1.png
    \o Untransformed
    \code
    <Image src="pics/qtlogo.png"/>
    \endcode
    \row
    \o \image declarative-qtlogo2.png
    \o Stretched
    \code
    <Image width="160" height="160" src="pics/qtlogo.png"/>
    \endcode
    \row
    \o \image declarative-qtlogo4.png
    \o Grid-scaled
    \code
    <Image scaleGrid.left="20" scaleGrid.right="10"
           scaleGrid.top="14" scaleGrid.bottom="14"
        width="160" height="160" src="pics/qtlogo.png"/>
    \endcode
    \row
    \o \image declarative-qtlogo3.png
    \o Tiled
    \code
    <Image tile="true"
        width="160" height="160" src="pics/qtlogo.png"/>
    \endcode
    \endtable
 */

/*!
    \internal
    \class QFxImage Image
    \brief The QFxImage class provides an image item that you can add to a QFxView.

    \ingroup group_coreitems

    Example:
    \code
    <Image src="pics/star.png"/>
    \endcode

    A QFxImage object can be instantiated in Qml using the tag \l Image.
*/

QFxImage::QFxImage(QFxItem *parent)
  : QFxItem(*(new QFxImagePrivate), parent)
{
    setOptions(SimpleItem | HasContents, true);
}

QFxImage::QFxImage(QFxImagePrivate &dd, QFxItem *parent)
  : QFxItem(dd, parent)
{
    setOptions(SimpleItem | HasContents, true);
}

QFxImage::~QFxImage()
{
    Q_D(const QFxImage);
    if(d->reply)
        d->reply->deleteLater();
}

/*!
    \property QFxImage::pixmap
    \brief the image displayed in this item.

    This property contains the image currently being displayed by this item,
    which may be an empty pixmap if nothing is currently displayed. If this
    property is set, the src property will be unset. This property is intended
    to be used only in C++, not in XML.
*/
QPixmap QFxImage::pixmap() const
{
    Q_D(const QFxImage);
    return d->_pix.pixmap();
}

void QFxImage::setPixmap(const QPixmap &pix)
{
    Q_D(QFxImage);
    d->url = QUrl();
    d->_pix.setPixmap(pix);
    d->_opaque=false;
    d->_pix.setOpaque(false);

    setImplicitWidth(d->_pix.width());
    setImplicitHeight(d->_pix.height());

#if defined(QFX_RENDER_OPENGL)
    d->_texDirty = true;
    d->_tex.clear();
#endif
    update();
}

/*!
    \qmlproperty int Image::scaleGrid.left
    \qmlproperty int Image::scaleGrid.right
    \qmlproperty int Image::scaleGrid.top
    \qmlproperty int Image::scaleGrid.bottom

    \target ImagexmlpropertiesscaleGrid

    A scale grid uses 4 grid lines (2 horizontal and 2 vertical) to break an image into 9 sections, as shown below:

    \image declarative-scalegrid.png

    When the image is scaled:
    \list
    \i the corners (sections 1, 3, 7, and 9) are not scaled at all
    \i the middle (section 5) is scaled both horizontally and vertically
    \i sections 2 and 8 are scaled horizontally
    \i sections 4 and 6 are scaled vertically
    \endlist

    Each scale grid property (left, right, top, and bottom) specifies an offset from the respective side. For example, \c scaleGrid.bottom="10" sets the bottom scale grid line 10 pixels up from the bottom of the image.

    A scale grid can also be specified using a
    \l {Image::src}{.sci file}.
*/
QFxScaleGrid *QFxImage::scaleGrid()
{
    Q_D(QFxImage);
    return d->scaleGrid();
}

/*!
    \qmlproperty bool Image::tile

    Set this property to enable image tiling.  Normally the Image element scales the
    bitmap file to its size.  If tiling is enabled, the bitmap is repeated as a set
    of unscaled tiles, clipped to the size of the Image.

    \code
    <Item>
        <Image src="tile.png" />
        <Image x="80" width="100" height="100" src="tile.png" />
        <Image x="190" width="100" height="100" tile="true" src="tile.png" />
    </Item>
    \endcode
    \image declarative-image_tile.png

    If both tiling and the scaleGrid are set, tiling takes precedence.
*/
bool QFxImage::isTiled() const
{
    Q_D(const QFxImage);
    return d->_tiled;
}

void QFxImage::setTiled(bool tile)
{
    Q_D(QFxImage);
    d->_tiled = tile;
}

/*!
    \qmlproperty bool Image::opaque

    Set this property if you know that the image is opaque to give your 
    application a significant performance boost.  

    \note
    This is a performance hint to Qt Declarative.  Unfortunately whether or not an image
    is opaque is not automatically detected.  Setting this property to true when
    the image is not opaque will lead to drawing artifacts.  However, leaving it as 
    false will always work correctly - although possibly not at maximum performance. 
 */

/*!
    \property QFxImage::opaque
    \brief whether the image is opaque (non-transparent).

    This property is provided purely for the purpose of optimization. An opaque
    image can be optimized more than a non-opaque one.
*/
bool QFxImage::isOpaque() const
{
    Q_D(const QFxImage);
    return d->_opaque;
}

void QFxImage::setOpaque(bool o)
{
    Q_D(QFxImage);
    if(o == d->_opaque)
        return;
    d->_opaque = o;
    d->_pix.setOpaque(o);
    update();
}

void QFxImage::componentComplete()
{
    QFxItem::componentComplete();
}

/*!
    \property QFxImage::scaleGrid
    \brief the 3x3 grid used to scale an image, excluding the corners.
*/

/*!
    \qmlproperty bool Image::smooth

    Set this property if you want the image to be smoothly filtered when scaled or
    transformed.  Smooth filtering gives better visual quality, but is slower.  If 
    the Image is displayed at its natural size, this property has no visual or 
    performance effect.

    \note Generally scaling artifacts are only visible if the image is stationary on 
    the screen.  A common pattern when animating an image is to disable smooth 
    filtering at the beginning of the animation and reenable it at the conclusion.
 */

/*!
    \property QFxImage::smooth
    \brief whether the image is smoothly transformed.

    This property is provided purely for the purpose of optimization. Turning
    smooth transforms off is faster, but looks worse; turning smooth 
    transformations on is slower, but looks better.

    By default smooth transformations are off.
*/
bool QFxImage::smoothTransform() const
{
    Q_D(const QFxImage);
    return d->_smooth;
}

void QFxImage::setSmoothTransform(bool s)
{
    Q_D(QFxImage);
    if(d->_smooth == s)
        return;
    d->_smooth = s;
    update();
}

void QFxImage::dump(int depth)
{
    Q_D(QFxImage);
    QByteArray ba(depth * 4, ' ');
    qWarning() << ba.constData() << "URL:" << d->url;
    QFxItem::dump(depth);
}

#if defined(QFX_RENDER_QPAINTER)
void QFxImage::paintContents(QPainter &p)
{
    Q_D(QFxImage);
    if(d->_pix.isNull())
        return;

    if(d->_smooth) {
        p.save();
        p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, d->_smooth);
    }

    QSimpleCanvasConfig::Image pix = d->_pix;

    if (d->_tiled) {
        p.save();
        p.setClipRect(0, 0, width(), height(), Qt::IntersectClip);
        QRect me = QRect(0, 0, width(), height());

        int pw = d->_pix.width();
        int ph = d->_pix.height();
        int yy = 0;

        while(yy < height()) {
            int xx = 0;
            while(xx < width()) {
                p.drawImage(xx, yy, d->_pix);
                xx += pw;
            }
            yy += ph;
        }

        p.restore();
    } else if (!d->_scaleGrid || d->_scaleGrid->isNull()) {
        if(width() != pix.width() || height() != pix.height()) {
            QTransform scale;
            scale.scale(width() / qreal(pix.width()), 
                        height() / qreal(pix.height()));
            QTransform old = p.transform();
            p.setWorldTransform(scale * old);
            p.drawImage(0, 0, pix);
            p.setWorldTransform(old);
        } else {
            p.drawImage(0, 0, pix);
        }
    } else {
        const int sgl = d->_scaleGrid->left();
        const int sgr = d->_scaleGrid->right();
        const int sgt = d->_scaleGrid->top();
        const int sgb = d->_scaleGrid->bottom();
        const int xSide = sgl + sgr;
        const int ySide = sgt + sgb;

        // Upper left
        if(sgt && sgl)
            p.drawImage(QRect(0, 0, sgl, sgt), pix, QRect(0, 0, sgl, sgt));
        // Upper middle
        if(pix.width() - xSide && sgt)
            p.drawImage(QRect(sgl, 0, width() - xSide, sgt), pix,
                        QRect(sgl, 0, pix.width() - xSide, sgt));
        // Upper right
        if(sgt && pix.width() - sgr) 
            p.drawImage(QPoint(width()-sgr, 0), pix,
                        QRect(pix.width()-sgr, 0, sgr, sgt));
        // Middle left
        if(sgl && pix.height() - ySide)
            p.drawImage(QRect(0, sgt, sgl, height() - ySide), pix,
                        QRect(0, sgt, sgl, pix.height() - ySide));

        // Middle
        if(pix.width() - xSide && pix.height() - ySide)
            p.drawImage(QRect(sgl, sgt, width() - xSide, height() - ySide), 
                        pix, 
                        QRect(sgl, sgt, pix.width() - xSide, pix.height() - ySide));
        // Middle right
        if(sgr && pix.height() - ySide)
            p.drawImage(QRect(width()-sgr, sgt, sgr, height() - ySide), pix,
                        QRect(pix.width()-sgr, sgt, sgr, pix.height() - ySide));
        // Lower left 
        if(sgl && sgr)
            p.drawImage(QPoint(0, height() - sgb), pix,
                        QRect(0, pix.height() - sgb, sgl, sgb));
        // Lower Middle
        if(pix.width() - xSide && sgb)
            p.drawImage(QRect(sgl, height() - sgb, width() - xSide, sgb), pix,
                        QRect(sgl, pix.height() - sgb, pix.width() - xSide, sgb));
        // Lower Right
        if(sgr && sgb)
            p.drawImage(QPoint(width()-sgr, height() - sgb), pix,
                        QRect(pix.width()-sgr, pix.height() - sgb, sgr, sgb));
    }

    if(d->_smooth) {
        p.restore();
    }
}
#elif defined(QFX_RENDER_OPENGL)
uint QFxImage::glSimpleItemData(float *vertices, float *texVertices,
                                GLTexture **texture, uint count)
{
    Q_D(QFxImage);

    if(d->_pix.isNull() || (d->_scaleGrid && !d->_scaleGrid->isNull()))
        return 0;

    if(count < 8) 
        return 8;

    d->checkDirty();

    float widthV = width();
    float heightV = height();

    vertices[0] = 0; vertices[1] = heightV;
    vertices[2] = widthV; vertices[3] = heightV;
    vertices[4] = 0; vertices[5] = 0;
    vertices[6] = widthV; vertices[7] = 0;

    *texture = &d->_tex;

    if(d->_tiled) {
        float tileWidth = widthV / d->_pix.width();
        float tileHeight = heightV / d->_pix.height();
        texVertices[0] = 0; texVertices[1] = 0;
        texVertices[2] = tileWidth; texVertices[3] = 0;
        texVertices[4] = 0; texVertices[5] = tileHeight;
        texVertices[6] = tileWidth; texVertices[7] = tileHeight;
    } else {
        texVertices[0] = 0; texVertices[1] = 0;
        texVertices[2] = 1; texVertices[3] = 0;
        texVertices[4] = 0; texVertices[5] = 1;
        texVertices[6] = 1; texVertices[7] = 1;
    }

    return 8;
}

void QFxImagePrivate::checkDirty()
{
    if(_texDirty && !_pix.isNull()) {
        _tex.setImage(_pix);
        _tex.setHorizontalWrap(GLTexture::Repeat);
        _tex.setVerticalWrap(GLTexture::Repeat);
    }
    _texDirty = false;
}

#if defined(QFX_RENDER_OPENGL2)
void QFxImage::paintGLContents(GLPainter &p)
{
    Q_D(QFxImage);
    if(d->_pix.isNull())
        return;

    QGLShaderProgram *shader = p.useTextureShader();

    bool restoreBlend = false;
    if(isOpaque() && p.activeOpacity == 1) {
        glDisable(GL_BLEND);
        restoreBlend = true;
    }

    if(d->_tiled || (!d->_scaleGrid || d->_scaleGrid->isNull())) {

        GLfloat vertices[8];
        GLfloat texVertices[8];
        GLTexture *tex = 0;

        QFxImage::glSimpleItemData(vertices, texVertices, &tex, 8);

        shader->setAttributeArray(SingleTextureShader::Vertices, vertices, 2);
        shader->setAttributeArray(SingleTextureShader::TextureCoords, texVertices, 2);

        glBindTexture(GL_TEXTURE_2D, tex->texture());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        shader->disableAttributeArray(SingleTextureShader::Vertices);
        shader->disableAttributeArray(SingleTextureShader::TextureCoords);

    } else {
        d->checkDirty();

        float imgWidth = d->_pix.width();
        float imgHeight = d->_pix.height();
        if(!imgWidth || !imgHeight) {
            if (restoreBlend)
                glEnable(GL_BLEND);
            return;
        }

        float widthV = width();
        float heightV = height();

        float texleft = 0;
        float texright = 1;
        float textop = 1;
        float texbottom = 0;
        float imgleft = 0;
        float imgright = widthV;
        float imgtop = 0;
        float imgbottom = heightV;

        const int sgl = d->_scaleGrid->left();
        const int sgr = d->_scaleGrid->right();
        const int sgt = d->_scaleGrid->top();
        const int sgb = d->_scaleGrid->bottom();

        if(sgl) {
            texleft = float(sgl) / imgWidth;
            imgleft = sgl;
        }
        if(sgr) {
            texright = 1. - float(sgr) / imgWidth;
            imgright = widthV - sgr;
        }
        if(sgt) {
            textop = 1. - float(sgb) / imgHeight;
            imgtop = sgt;
        }
        if(sgb) {
            texbottom = float(sgt) / imgHeight;
            imgbottom = heightV - sgb;
        }

        float vert1[] = { 0, 0, 
                          0, imgtop,
                          imgleft, 0, 
                          imgleft, imgtop,
                          imgright, 0,
                          imgright, imgtop,
                          widthV, 0,
                          widthV, imgtop };
        float tex1[] = { 0, 1, 
                         0, textop,
                         texleft, 1,
                         texleft, textop,
                         texright, 1,
                         texright, textop,
                         1, 1,
                         1, textop };
        float vert2[] = { 0, imgtop,
                          0, imgbottom,
                          imgleft, imgtop,
                          imgleft, imgbottom,
                          imgright, imgtop,
                          imgright, imgbottom,
                          widthV, imgtop,
                          widthV, imgbottom };
        float tex2[] = { 0, textop,
                         0, texbottom,
                         texleft, textop,
                         texleft, texbottom,
                         texright, textop,
                         texright, texbottom,
                         1, textop,
                         1, texbottom };
        float vert3[] = { 0, imgbottom,
                          0, heightV,
                          imgleft, imgbottom,
                          imgleft, heightV,
                          imgright, imgbottom,
                          imgright, heightV,
                          widthV, imgbottom,
                          widthV, heightV };
        float tex3[] = { 0, texbottom,
                         0, 0,
                         texleft, texbottom,
                         texleft, 0,
                         texright, texbottom,
                         texright, 0,
                         1, texbottom,
                         1, 0 };

        glBindTexture(GL_TEXTURE_2D, d->_tex.texture());

        shader->setAttributeArray(SingleTextureShader::Vertices, vert1, 2);
        shader->setAttributeArray(SingleTextureShader::TextureCoords, tex1, 2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
        shader->setAttributeArray(SingleTextureShader::Vertices, vert2, 2);
        shader->setAttributeArray(SingleTextureShader::TextureCoords, tex2, 2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
        shader->setAttributeArray(SingleTextureShader::Vertices, vert3, 2);
        shader->setAttributeArray(SingleTextureShader::TextureCoords, tex3, 2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);

        shader->disableAttributeArray(SingleTextureShader::Vertices);
        shader->disableAttributeArray(SingleTextureShader::TextureCoords);
    }

    if (restoreBlend)
        glEnable(GL_BLEND);
}
#elif defined(QFX_RENDER_OPENGL1)
void QFxImage::paintGLContents(GLPainter &p)
{
    Q_D(QFxImage);
    if(d->_pix.isNull())
        return;

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(p.activeTransform.data());

    bool restoreBlend = false;
    if(isOpaque() && p.activeOpacity == 1) {
        glDisable(GL_BLEND);
        restoreBlend = true;
    }

    glEnable(GL_TEXTURE_2D);
    if(p.activeOpacity == 1.) {
        GLint i = GL_REPLACE;
        glTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &i);
    } else {
        GLint i = GL_MODULATE;
        glTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &i);
        glColor4f(1, 1, 1, p.activeOpacity);
    }

    if(d->_tiled || !d->_scaleGrid || d->_scaleGrid->isNull()) {

        GLfloat vertices[8];
        GLfloat texVertices[8];
        GLTexture *tex = 0;

        QFxImage::glSimpleItemData(vertices, texVertices, &tex, 8);

        glBindTexture(GL_TEXTURE_2D, tex->texture());

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(2, GL_FLOAT, 0, vertices);
        glTexCoordPointer(2, GL_FLOAT, 0, texVertices);


        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);

    } else {
        d->checkDirty();

        float imgWidth = d->_pix.width();
        float imgHeight = d->_pix.height();
        if(!imgWidth || !imgHeight) {
            if (restoreBlend)
                glEnable(GL_BLEND);
            return;
        }

        float widthV = width();
        float heightV = height();

        float texleft = 0;
        float texright = 1;
        float textop = 1;
        float texbottom = 0;
        float imgleft = 0;
        float imgright = widthV;
        float imgtop = 0;
        float imgbottom = heightV;

        const int sgl = d->_scaleGrid->left();
        const int sgr = d->_scaleGrid->right();
        const int sgt = d->_scaleGrid->top();
        const int sgb = d->_scaleGrid->bottom();

        if(sgl) {
            texleft = float(sgl) / imgWidth;
            imgleft = sgl;
        }
        if(sgr) {
            texright = 1. - float(sgr) / imgWidth;
            imgright = widthV - sgr;
        }
        if(sgt) {
            textop = 1. - float(sgb) / imgHeight;
            imgtop = sgt;
        }
        if(sgb) {
            texbottom = float(sgt) / imgHeight;
            imgbottom = heightV - sgb;
        }

        float vert1[] = { 0, 0, 
                          0, imgtop,
                          imgleft, 0, 
                          imgleft, imgtop,
                          imgright, 0,
                          imgright, imgtop,
                          widthV, 0,
                          widthV, imgtop };
        float tex1[] = { 0, 1, 
                         0, textop,
                         texleft, 1,
                         texleft, textop,
                         texright, 1,
                         texright, textop,
                         1, 1,
                         1, textop };
        float vert2[] = { 0, imgtop,
                          0, imgbottom,
                          imgleft, imgtop,
                          imgleft, imgbottom,
                          imgright, imgtop,
                          imgright, imgbottom,
                          widthV, imgtop,
                          widthV, imgbottom };
        float tex2[] = { 0, textop,
                         0, texbottom,
                         texleft, textop,
                         texleft, texbottom,
                         texright, textop,
                         texright, texbottom,
                         1, textop,
                         1, texbottom };
        float vert3[] = { 0, imgbottom,
                          0, heightV,
                          imgleft, imgbottom,
                          imgleft, heightV,
                          imgright, imgbottom,
                          imgright, heightV,
                          widthV, imgbottom,
                          widthV, heightV };
        float tex3[] = { 0, texbottom,
                         0, 0,
                         texleft, texbottom,
                         texleft, 0,
                         texright, texbottom,
                         texright, 0,
                         1, texbottom,
                         1, 0 };

        glBindTexture(GL_TEXTURE_2D, d->_tex.texture());

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(2, GL_FLOAT, 0, vert1);
        glTexCoordPointer(2, GL_FLOAT, 0, tex1);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
        glVertexPointer(2, GL_FLOAT, 0, vert2);
        glTexCoordPointer(2, GL_FLOAT, 0, tex2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
        glVertexPointer(2, GL_FLOAT, 0, vert3);
        glTexCoordPointer(2, GL_FLOAT, 0, tex3);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
    }

    if (restoreBlend)
        glEnable(GL_BLEND);
}
#endif

#endif

QString QFxImage::propertyInfo() const
{
    Q_D(const QFxImage);
    return d->url.toString();
}

QFxImage::Status QFxImage::status() const
{
    Q_D(const QFxImage);
    return d->status;
}

/*!
    \qmlproperty string Image::src

    Image can handle any image format supported by Qt, loaded from any URL scheme supported by Qt.

    It can also handle .sci files, which are a Qml-specific format. A .sci file uses a simple text-based format that specifies
    \list
    \i the grid lines describing a \l {Image::scaleGrid.left}{scale grid}.
    \i an image file.
    \endlist

    The following .sci file sets grid line offsets of 10 on each side for the image \c picture.png:
    \code
    gridLeft: 10
    gridTop: 10
    gridBottom: 10
    gridRight: 10
    imageFile: picture.png
    \endcode

    The URL may be absolute, or relative to the URL of the component.
*/

/*!
    \property QFxImage::src
    \brief the url of the image to be displayed in this item.

    The content specified can be of any image type loadable by QImage. Alternatively,
    you can specify an sci format file, which specifies both an image and it's scale grid.
*/
QString QFxImage::source() const
{
    Q_D(const QFxImage);
    return d->source;
}

void QFxImage::setSource(const QString &url)
{
    Q_D(QFxImage);
    if(url == d->source)
        return;

    if(d->reply) {
        d->reply->deleteLater();
        d->reply = 0;
    }

    if (!d->url.isEmpty())
        QFxPixmap::cancelGet(d->url, this, SLOT(requestFinished()));
    if (!d->sciurl.isEmpty())
        QFxPixmap::cancelGet(d->sciurl, this, SLOT(requestFinished()));

    d->source = url;
    d->url = qmlContext(this)->resolvedUrl(url);
    d->sciurl = QUrl();

    if(url.isEmpty()) {
        setPixmap(QPixmap());
        d->status = Idle;
    } else {
        d->status = Loading;
        if (d->url.path().endsWith(QLatin1String(".sci"))) {
#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
            if (d->url.scheme() == QLatin1String("file")) {
                QFile file(d->url.toLocalFile());
                file.open(QIODevice::ReadOnly);
                setGridScaledImage(QFxGridScaledImage(&file));
            } else
#endif
            {
                QNetworkRequest req(d->url);
                req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
                d->reply = qmlEngine(this)->networkAccessManager()->get(req);
                QObject::connect(d->reply, SIGNAL(finished()), 
                                 this, SLOT(sciRequestFinished()));
            }
        } else {
            QFxPixmap::get(qmlEngine(this), d->url, this, SLOT(requestFinished()));
        }
    }

    emit statusChanged(d->status);
}

void QFxImage::requestFinished()
{
    Q_D(QFxImage);
    if (d->url.path().endsWith(QLatin1String(".sci"))) {
        d->_pix = QFxPixmap(d->sciurl);
    } else {
        d->_pix = QFxPixmap(d->url);
        d->_pix.setOpaque(d->_opaque);
        setOptions(QFxImage::SimpleItem, true);
    }
    setImplicitWidth(d->_pix.width());
    setImplicitHeight(d->_pix.height());

    d->status = Idle;
#if defined(QFX_RENDER_OPENGL)
    d->_texDirty = true;
    d->_tex.clear();
#endif
    emit statusChanged(d->status);
    emit sourceChanged(d->source);
    update();
}

void QFxImage::sciRequestFinished()
{
    Q_D(QFxImage);
    if(d->reply->error() != QNetworkReply::NoError) {
        d->status = Error;
        d->reply->deleteLater();
        d->reply = 0;
        emit statusChanged(d->status);
    } else {
        QFxGridScaledImage sci(d->reply);
        d->reply->deleteLater();
        d->reply = 0;
        setGridScaledImage(sci);
    }
}


void QFxImage::setGridScaledImage(const QFxGridScaledImage& sci)
{
    Q_D(QFxImage);
    if(!sci.isValid()) {
        d->status = Error;
        emit statusChanged(d->status);
    } else {
        d->sciurl = d->url.resolved(QUrl(sci.pixmapUrl()));
        QFxPixmap::get(qmlEngine(this), d->sciurl, this, SLOT(requestFinished()));
        QFxScaleGrid *sg = scaleGrid();
        sg->setTop(sci.gridTop());
        sg->setBottom(sci.gridBottom());
        sg->setLeft(sci.gridLeft());
        sg->setRight(sci.gridRight());
        setOptions(QFxImage::SimpleItem, false);
    }
}


QT_END_NAMESPACE
