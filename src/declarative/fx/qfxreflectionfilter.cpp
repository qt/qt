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

#include "qfxreflectionfilter.h"

#if defined(QFX_RENDER_OPENGL2)
#include <glsave.h>
#include <QtOpenGL/qglframebufferobject.h>
#include <glbasicshaders.h>
#include <gltexture.h>
#endif

QT_BEGIN_NAMESPACE
class QFxReflectionFilterPrivate
{
public:
    QFxReflectionFilterPrivate()
        : alpha(1), height(-1), offset(0), scale(1)
    {
    }
    qreal alpha;
    int height;
    int offset;
    qreal scale;
};

/*!
    \qmlclass Reflection
    \inherits Filter
    \brief The Reflection filter reflects an item and its contents.

    Here is an example of various Reflections applied to an image.

    \qml
HorizontalLayout {
    Image {
        source: "icon.png"
        filter: Reflection { }
    }
    Image {
        source: "icon.png"
        filter: Reflection { offset: 1 }
    }
    Image {
        source: "icon.png"
        filter: Reflection { offset: 1; alpha: 0.5 }
    }
    Image {
        source: "icon.png"
        filter: Reflection { offset: 1; alpha: 0.5; height: 50 }
    }
    Image {
        source: "icon.png"
        filter: Reflection { offset: 1; alpha: 0.5; height: 50; scale: 0.5 }
    }
}
    \endqml

    \image reflection_example.png

    Reflection is only supported when Qt Declarative is compiled for OpenGL ES 2.0.
    Otherwise the Reflection filter has no effect.
*/

/*!
    \internal
    \class QFxReflectionFilter
    \ingroup group_effects
    \brief The QFxReflectionFilter class allows you to add a reflection to an item.
*/
QFxReflectionFilter::QFxReflectionFilter(QObject *parent)
: QSimpleCanvasFilter(parent), d(new QFxReflectionFilterPrivate)
{
}

QFxReflectionFilter::~QFxReflectionFilter()
{
    delete d; d = 0;
}

/*!
    \property QFxReflectionFilter::alpha
    \brief the starting opacity of the reflection.

    The starting opacity is the opacity closest to the item. The opacity will fade
    from this value to zero over the height of the reflection.
*/
qreal QFxReflectionFilter::alpha() const
{
    return d->alpha;
}

void QFxReflectionFilter::setAlpha(qreal a)
{
    if (d->alpha == a) return;
    d->alpha = a;
    emit alphaChanged(a);
    update();
}

/*!
    \qmlproperty int Reflection::height

    The height property controls how much of the item, in pixels, to reflect.
    If it is set to the default value of -1, the whole item is reflected.  If 
    it is set to 50, the bottom 50 pixels of the item are reflected.  Data 
    binding could be used to reflect a percentage of the item.

    \qml 
Image {
    id: myImage
    source: "album.png"
    filter: Reflection {
        height: myImage.height * 0.5
    }
}
    \endqml
 */
/*!
    \qmlproperty int Reflection::offset

    The offset controls how far from the base of the item, in pixels, the 
    start of the reflection is placed.  This can be used to create a nice
    sliver of space between the item and its reflection or for more advanced
    effects.

    The default offset is 0 pixels.
*/

/*!
    \qmlproperty real Reflection::alpha

    The alpha value controls the starting opacity of the reflected item.  If
    set to the default value of 1, the reflected item starts completely opaque
    and gradually fades to completely transparent.  If set to less than one, the
    reflection starts out partially transparent as though the item was sitting
    on a visually less reflective surface.

    Valid values are from 0 (which would be silly, but is allowed) to 1.
*/
/*!
    \qmlproperty real Reflection::scale

    When set to the default value of 1, the reflection is a 1:1 reflection of
    the item.  That is, each horizontal pixel in the item corresponds to one
    horizontal pixel in the reflection.

    When set a value other than 1, the reflection is scaled acordingly - less
    than 1 scales it down and greater than 1 scales it up.  The scale is applied
    after the height parameter and does not effect the reflection offset.
*/

/*!
    \property QFxReflectionFilter::height
    \brief the height of the reflection, in pixels.
*/
int QFxReflectionFilter::height() const
{
    return d->height;
}

void QFxReflectionFilter::setHeight(int h)
{
    if (d->height == h) return;
    d->height = h;
    emit heightChanged(h);
    update();
}

/*!
    \property QFxReflectionFilter::offset
    \brief the distance of the reflection from the item, in pixels.
*/
int QFxReflectionFilter::offset()
{
    return d->offset;
}

void QFxReflectionFilter::setOffset(int o)
{
    if (d->offset == o) return;
    d->offset = o;
    emit offsetChanged(o);
    update();
}

/*!
    \property QFxReflectionFilter::scale
    \brief the scale of the reflection relative to the item.
*/
qreal QFxReflectionFilter::scale() const
{
    return d->scale;
}

void QFxReflectionFilter::setScale(qreal s)
{
    if (d->scale == s) return;
    d->scale = s;
    emit scaleChanged(s);
    update();
}

static inline float floatmin(float a, float b)
{
    return (a < b)?a:b;
}

void QFxReflectionFilter::filterGL(QSimpleCanvasItem::GLPainter &p)
{
#if defined(QFX_RENDER_OPENGL2)
    QSimpleCanvasItem *item = this->item();

    QRect r = item->itemBoundingRect();
    if (r.isEmpty())
        return;
    float width = r.width();
    float height = r.height();

    float refHeight = height;
    if (d->height > 0)
        refHeight = floatmin(height, d->height);

    QSimpleCanvas::Matrix simpMat;
    QSimpleCanvasItem *simpItem = 0;
    if (isSimpleItem(&simpItem, &simpMat) &&
       simpItem->glSimpleItemData(0, 0, 0, 0)) {

        GLfloat vertices[8];
        GLfloat texVertices[8];
        GLTexture *texture = 0;

        simpItem->glSimpleItemData(vertices, texVertices, &texture, 8);

        GLfloat opacity[4];

        float invRefHeight = 1. / refHeight;
        for (int ii = 0; ii < 4; ++ii) {
            float vertex = vertices[ii * 2 + 1];
            float o = (1. - (height - vertex) * invRefHeight);
            opacity[ii] = o * d->alpha * p.activeOpacity;
        }

        QSimpleCanvas::Matrix trans = p.activeTransform;
        trans.rotate(180, 1, 0, 0);
        trans.translate(0, -r.height() - d->offset);
        if (d->scale != 1)
            trans.scale(1, d->scale, 1);
        trans.translate(0, -r.height());
        trans *= simpMat;

        glBindTexture(GL_TEXTURE_2D, texture->texture());

        SingleTextureVertexOpacityShader *shader =
            item->basicShaders()->singleTextureVertexOpacity();
        shader->enable();
        shader->setTransform(trans);
        shader->setAttributeArray(SingleTextureVertexOpacityShader::Vertices, vertices, 2);
        shader->setAttributeArray(SingleTextureVertexOpacityShader::TextureCoords, texVertices, 2);
        shader->setAttributeArray(SingleTextureVertexOpacityShader::OpacityCoords, opacity, 1);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        shader->disableAttributeArray(SingleTextureVertexOpacityShader::Vertices);
        shader->disableAttributeArray(SingleTextureVertexOpacityShader::TextureCoords);
        shader->disableAttributeArray(SingleTextureVertexOpacityShader::OpacityCoords);

    } else {
        QGLFramebufferObject *fbo = renderToFBO();

        float texWidth = width / float(fbo->width());
        float texHeight = refHeight / float(fbo->height());

        GLfloat invVertices[] = { width, height + d->scale * refHeight + d->offset,
                                  0, height + d->scale * refHeight + d->offset,
                                  width, height + d->offset, 
                                  0, height + d->offset };
        GLfloat invTexVertices[] = { texWidth, texHeight, 
                                     0, texHeight,
                                     texWidth, 0,
                                     0, 0 };
        GLfloat invOpacity[] = { 0, 0, d->alpha * p.activeOpacity, d->alpha * p.activeOpacity};

        glBindTexture(GL_TEXTURE_2D, fbo->texture());

        SingleTextureVertexOpacityShader *shader =
            item->basicShaders()->singleTextureVertexOpacity();
        shader->enable();
        shader->setTransform(p.activeTransform);
        shader->setAttributeArray(SingleTextureVertexOpacityShader::Vertices, invVertices, 2);
        shader->setAttributeArray(SingleTextureVertexOpacityShader::TextureCoords, invTexVertices, 2);
        shader->setAttributeArray(SingleTextureVertexOpacityShader::OpacityCoords, invOpacity, 1);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        shader->disableAttributeArray(SingleTextureVertexOpacityShader::Vertices);
        shader->disableAttributeArray(SingleTextureVertexOpacityShader::TextureCoords);
        shader->disableAttributeArray(SingleTextureVertexOpacityShader::OpacityCoords);

        releaseFBO(fbo);
    }

    renderToScreen();

#else
    Q_UNUSED(p);
#endif
}

QRectF QFxReflectionFilter::itemBoundingRect(const QRectF &r) const
{
    QRectF rv = r;
    rv |= r.translated(0, r.height() + d->offset);
    return rv;
}

QML_DEFINE_TYPE(QFxReflectionFilter,Reflection)
QT_END_NAMESPACE
