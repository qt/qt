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

#include "qfxshadowfilter.h"

#if defined(QFX_RENDER_OPENGL2)
#include <glsave.h>
#include <QtOpenGL/qglframebufferobject.h>
#include <glbasicshaders.h>
#endif

QT_BEGIN_NAMESPACE

class QFxShadowFilterPrivate
{
public:
    QFxShadowFilterPrivate()
        : x(0), y(0)
    {
    }

    int x;
    int y;
};

/*!
    \qmlclass Shadow
    \brief The Shadow filter casts a drop shadow.
    \inherits Filter

    Shadows work on all visual elements - including transparent and masked 
    images.

    \table
    \row
    \o \image shadow_example.png
    \o
    \qml
Rect {
    radius: 5
    color: "lightsteelblue"
    width: 100
    height: 100
    filter: Shadow {
        yOffset: 8
        xOffset: 8
    }
}

Image {
    source: "pics/qtlogo.png"
    filter: Shadow {
        yOffset: 8
        xOffset: 8
    }
}
    \endqml
    \endtable

    Shadows are only supported when Qt Qt Declarative is compiled for OpenGL ES 2.0.
    Otherwise the Shadow filter has no effect.
*/

/*!
    \internal
    \class QFxShadowFilter
    \ingroup group_effects
    \brief The QFxShadowFilter class allows you to add a shadow to an item.
*/

QFxShadowFilter::QFxShadowFilter(QObject *parent)
: QSimpleCanvasFilter(parent), d(new QFxShadowFilterPrivate)
{
}

QFxShadowFilter::~QFxShadowFilter()
{
    delete d; d = 0;
}

/*!
    \qmlproperty int Shadow::xOffset
    \qmlproperty int Shadow::yOffset

    Specify the x and y offset of the shadow relative to the item.
*/

int QFxShadowFilter::xOffset() const
{
    return d->x;
}

/*!
    \property QFxShadowFilter::xOffset
    \brief the x offset of the shadow relative to the item.
*/
void QFxShadowFilter::setXOffset(int offset)
{
    if (d->x == offset) return;
    d->x = offset;
    emit offsetChanged(d->x, d->y);
}

/*!
    \property QFxShadowFilter::yOffset
    \brief the y offset of the shadow relative to the item.
*/
int QFxShadowFilter::yOffset() const
{
    return d->y;
}

void QFxShadowFilter::setYOffset(int offset)
{
    if (d->y == offset) return;
    d->y = offset;
    emit offsetChanged(d->x, d->y);
}

QRectF QFxShadowFilter::itemBoundingRect(const QRectF &r) const
{
    QRectF rv = r;
    rv |= r.translated(xOffset(), yOffset());
    return rv;
}

void QFxShadowFilter::filterGL(QSimpleCanvasItem::GLPainter &p)
{
#if defined(QFX_RENDER_OPENGL2)

    QSimpleCanvasItem *item = this->item();

    QRect r = item->itemBoundingRect();

    QGLFramebufferObject *fbo = renderToFBO();

    float width = r.width();
    float height = r.height();

    float texWidth = width / float(fbo->width());
    float texHeight = height / float(fbo->height());

    GLfloat vertices[] = { d->x, height + d->y, 
                           width + d->x, height + d->y, 
                           d->x, d->y, 
                           d->x + width, d->y };
    GLfloat texVertices[] = { 0, 0, 
                              texWidth, 0, 
                              0, texHeight,
                              texWidth, texHeight };

    SingleTextureShadowShader *shader = item->basicShaders()->singleTextureShadow();
    shader->enable();
    shader->setOpacity(0.8 * p.activeOpacity);
    shader->setTransform(p.activeTransform);

    shader->setAttributeArray(SingleTextureShadowShader::Vertices, vertices, 2);
    shader->setAttributeArray(SingleTextureShadowShader::TextureCoords, texVertices, 2);

    glBindTexture(GL_TEXTURE_2D, fbo->texture());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    shader->disableAttributeArray(SingleTextureShadowShader::Vertices);
    shader->disableAttributeArray(SingleTextureShadowShader::TextureCoords);

    releaseFBO(fbo);

    renderToScreen();
#else
    Q_UNUSED(p);
#endif
}

QML_DEFINE_TYPE(QFxShadowFilter,Shadow)

QT_END_NAMESPACE
