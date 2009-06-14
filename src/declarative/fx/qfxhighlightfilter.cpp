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

#include "qfxhighlightfilter.h"
#include <qfxpixmap.h>
#include <qmlcontext.h>

#if defined(QFX_RENDER_OPENGL2)
#include <gltexture.h>
#include <glbasicshaders.h>
#include <QtOpenGL/qglframebufferobject.h>
#include <glsave.h>
#endif

QT_BEGIN_NAMESPACE

class QFxHighlightFilterPrivate
{
public:
    QFxHighlightFilterPrivate()
        : xOffset(0), yOffset(0), tiled(false) {}

    QUrl url;
    int xOffset;
    int yOffset;
    bool tiled;
#if defined(QFX_RENDER_OPENGL2)
    GLTexture tex;
#endif
};

/*!
    \qmlclass Highlight
    \brief The Highlight filter adds a highlight to an item.
    \inherits Filter

    \qml
Text {
    id: highlighttext
    color: "red"
    font.size: 32
    text: "Highlight"
    filter: Highlight {
        source: "pics/highlight.png"
        xOffset: NumericAnimation {
            running: true
            repeat: true
            from: 320
            to: -320
            duration: 2000
        }
    }
}
    \endqml
    \image highlight.gif

    Highlighting is only supported when Qt Declarative is compiled for OpenGL ES 2.0.
    Otherwise the Highlight filter has no effect.
*/

/*!
    \internal
    \class QFxHighlightFilter
    \ingroup group_effects
    \brief The QFxHightlightFilter class allows you to add a highlight to an item.
*/

QFxHighlightFilter::QFxHighlightFilter(QObject *parent)
: QSimpleCanvasFilter(parent), d(new QFxHighlightFilterPrivate)
{
#if defined(QFX_RENDER_OPENGL2)
    d->tex.setHorizontalWrap(GLTexture::ClampToEdge);
    d->tex.setVerticalWrap(GLTexture::ClampToEdge);
#endif
}

QFxHighlightFilter::~QFxHighlightFilter()
{
    delete d;
    d = 0;
}

/*!
    \qmlproperty string Highlight::source
    This property holds the URL of the image to be used as the highlight.
*/

/*!
    \property QFxHighlightFilter::source
    \brief the URL of the image to be used as the highlight.
*/
QUrl QFxHighlightFilter::source() const
{
    return d->url;
}

void QFxHighlightFilter::imageLoaded()
{
    QPixmap img = QFxPixmap(d->url);
#if defined(QFX_RENDER_OPENGL2)
    if (!img.isNull()) 
        d->tex.setImage(img.toImage());
#endif
    emit sourceChanged(d->url);
    update();
}

void QFxHighlightFilter::setSource(const QUrl &f)
{
    if (d->url == f)
        return;
    if (!d->url.isEmpty())
        QFxPixmap::cancelGet(d->url, this);
    Q_ASSERT(!f.isRelative());
    d->url = f;
#if defined(QFX_RENDER_OPENGL2)
    d->tex.clear();
#endif
    if (!f.isEmpty())
        QFxPixmap::get(qmlEngine(this), d->url, this, SLOT(imageLoaded()));
    else
        emit sourceChanged(d->url);
}

/*!
    \qmlproperty bool Highlight::tiled
    This property holds whether or not the highlight should be tiled.
*/

/*!
    \property QFxHighlightFilter::tiled
    \brief whether or not the highlight should be tiled.
*/
bool QFxHighlightFilter::tiled() const
{
    return d->tiled;
}

void QFxHighlightFilter::setTiled(bool t)
{
    if (t == d->tiled)
        return;

    d->tiled = t;

#if defined(QFX_RENDER_OPENGL2)
    if (d->tiled) {
        d->tex.setHorizontalWrap(GLTexture::ClampToEdge);
        d->tex.setVerticalWrap(GLTexture::ClampToEdge);
    } else {
        d->tex.setHorizontalWrap(GLTexture::Repeat);
        d->tex.setVerticalWrap(GLTexture::Repeat);
    }
#endif

    emit tiledChanged(d->tiled);
}

/*!
    \qmlproperty int Highlight::xOffset
    \qmlproperty int Highlight::yOffset
    These properties hold the position of the highlight, relative to the item.
*/

/*!
    \property QFxHighlightFilter::xOffset
    \brief the x position of the highlight, relative to the item.
*/
int QFxHighlightFilter::xOffset() const
{
    return d->xOffset;
}

void QFxHighlightFilter::setXOffset(int x)
{
    if (x == d->xOffset)
        return;

    d->xOffset = x;
    emit offsetChanged(d->xOffset, d->yOffset);
#if defined(QFX_RENDER_OPENGL2)
    update();
#endif
}

/*!
    \property QFxHighlightFilter::yOffset
    \brief the y position of the highlight, relative to the item.
*/
int QFxHighlightFilter::yOffset() const
{
    return d->yOffset;
}

void QFxHighlightFilter::setYOffset(int y)
{
    if (y == d->yOffset)
        return;

    d->yOffset = y;
    emit offsetChanged(d->xOffset, d->yOffset);
#if defined(QFX_RENDER_OPENGL2)
    update();
#endif
}

void QFxHighlightFilter::filterGL(QSimpleCanvasItem::GLPainter &p)
{
#if defined(QFX_RENDER_OPENGL2)
    if (d->tex.isNull()) {
        renderToScreen();
    } else {
        QSimpleCanvasItem *item = this->item();

        QRect r = item->itemBoundingRect();

        QGLFramebufferObject *fbo = renderToFBO();

        float width = r.width();
        float height = r.height();

        float texWidth = width / float(fbo->width());
        float texHeight = height / float(fbo->height());

        GLfloat vert[] = { 0, height, 
                           width, height, 
                           0, 0, 
                           width, 0 };
        GLfloat texVert[] = { 0, 0, 
                              texWidth, 0, 
                              0, texHeight, 
                              texWidth, texHeight };
        float texXOffset = 0;
        float texYOffset = 0;

        if (xOffset())
            texXOffset = float(xOffset()) / float(d->tex.width());
        if (yOffset())
            texYOffset = float(yOffset()) / float(d->tex.height());

        GLfloat addTexVert[] = { texXOffset, texYOffset, 
                                 1 + texXOffset, texYOffset, 
                                 texXOffset, 1 + texYOffset, 
                                 1 + texXOffset, 1 + texYOffset };

        glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fbo->texture());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, d->tex.texture()); 
    
        DualTextureAddShader *shader = item->basicShaders()->dualTextureAdd();
        shader->enable();
        shader->setTransform(p.activeTransform);
        shader->setOpacity(p.activeOpacity);

        shader->setAttributeArray(DualTextureAddShader::Vertices, vert, 2);
        shader->setAttributeArray(DualTextureAddShader::TextureCoords, texVert, 2);
        shader->setAttributeArray(DualTextureAddShader::AddTextureCoords, addTexVert, 2);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        shader->disableAttributeArray(DualTextureAddShader::Vertices);
        shader->disableAttributeArray(DualTextureAddShader::TextureCoords);
        shader->disableAttributeArray(DualTextureAddShader::AddTextureCoords);

        glActiveTexture(GL_TEXTURE0);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
        releaseFBO(fbo);
    }
#else
    Q_UNUSED(p);
#endif
}

QML_DEFINE_TYPE(QFxHighlightFilter,Highlight)

QT_END_NAMESPACE
