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

#include "qfxblurfilter.h"
#include <private/qsimplecanvasitem_p.h>

#if defined(QFX_RENDER_OPENGL2)
#include <glsave.h>
#include <QtOpenGL/qglframebufferobject.h>
#include <glbasicshaders.h>
#endif

QT_BEGIN_NAMESPACE
class QFxBlurFilterPrivate
{
public:
    QFxBlurFilterPrivate()
        : radius(0)
    {
    }
    qreal radius;
};

/*!
    \qmlclass Blur
    \brief The Blur filter blurs an item and its contents.
    \inherits Filter

    Blurring reduces the clarity of a visual item.  The following example
    shows an icon at a blur radius of 0, 5 and 10.

    \table
    \row
    \o
    \qml
HorizontalLayout {
    Image {
        source: "icon.png"
        filter: Blur { radius: 0 }
    }
    Image {
        source: "icon.png"
        filter: Blur { radius: 5 }
    }
    Image {
        source: "icon.png"
        filter: Blur { radius: 10 }
    }
}
    \endqml
    \row
    \o \image blur_example.png
    \endtable

    Bluring is only supported when Qt Declarative is compiled for OpenGL ES 2.0.
    Otherwise the Blur filter has no effect.
    */
/*!
    \internal
    \class QFxBlurFilter
    \ingroup group_effects
    \brief The QFxBlurFilter class allows you to blur an item.
*/

QFxBlurFilter::QFxBlurFilter(QObject *parent)
: QSimpleCanvasFilter(parent), d(new QFxBlurFilterPrivate)
{
}

QFxBlurFilter::~QFxBlurFilter()
{
    delete d; d = 0;
}

/*!
    \qmlproperty real Blur::radius

    Sets the blur kernel radius.
    The larger the radius the more blurry the item will appear.
    A radius of 0 (or less) is equivalent to no blur.
 */

/*!
    \property QFxBlurFilter::radius
    \brief the radius of the blur.
*/
qreal QFxBlurFilter::radius() const
{
    return d->radius;
}

void QFxBlurFilter::setRadius(qreal radius)
{
    if (d->radius == radius) return;
    d->radius = radius;
    emit radiusChanged(radius);
    update();
}

QRectF QFxBlurFilter::itemBoundingRect(const QRectF &r) const
{
    QRectF rv = r;
    if (d->radius > 0)
        rv.adjust(-d->radius, -d->radius, d->radius, d->radius);
    return rv;
}

#include <math.h>
void QFxBlurFilter::filterGL(QSimpleCanvasItem::GLPainter &p)
{
#if defined(QFX_RENDER_OPENGL2)
#if 1
    if (d->radius <= 0) {
        renderToScreen();
        return;
    }
    float radius = d->radius;
    QSimpleCanvasItem *item = this->item();

    QRect r = item->itemBoundingRect();
    float blurScale = 1.0;
    QRect tr = QRect(QPoint(0, 0), r.size() * blurScale);
    radius *= blurScale;

    QGLFramebufferObject *fbo = renderToFBO(blurScale);
    if (!fbo) 
        return;

    float height = r.height();
    float width = r.width();

    float texWidth = float(tr.width()) / float(fbo->width());
    float texHeight = float(tr.height()) / float(fbo->height());

    int steps = int(::ceil(radius));
    int dispSteps = int(::ceil(d->radius));
    float xstep = texWidth * radius / float(steps * fbo->width());
    float xinc = steps / float(fbo->width());

    glDisable(GL_BLEND);

    // Render x pass
    QSize xSize(tr.width() + 2 * steps, tr.height());
    QGLFramebufferObject *xBlur = acquireFBO(xSize);
    float xWidth = float(xSize.width()) / float(xBlur->width());
    float xHeight = float(xSize.height()) / float(xBlur->height());
    {
        xBlur->bind();

        GLSaveViewport sv; GLSaveScissor ss;
        glClearColor(0,0,0,0);
        glDisable(GL_SCISSOR_TEST);
        glViewport(0, 0, xBlur->width(), xBlur->height());
        glClear(GL_COLOR_BUFFER_BIT); 

        float vert[] = { 0, xHeight,
                         xWidth, xHeight,
                         0, 0,
                         xWidth, 0 };
        float texVert[] = { -xinc, 0,
                            texWidth + xinc, 0,
                            -xinc, texHeight,
                            texWidth + xinc, texHeight };

        QMatrix4x4 trans;
        trans.translate(-1, -1);
        trans.scale(2, 2);
        BlurTextureShader *shader = item->basicShaders()->blurTexture();
        shader->enable();
        shader->setTransform(trans);
        if (steps > 1) {
            shader->setStep(xstep * 2);
            shader->setSteps(steps / 2);
        } else {
            shader->setStep(xstep);
            shader->setSteps(steps);
        }
        shader->setMode(BlurTextureShader::Horizontal);

        glBindTexture(GL_TEXTURE_2D, fbo->texture());

        shader->setAttributeArray(BlurTextureShader::Vertices, vert, 2);
        shader->setAttributeArray(BlurTextureShader::TextureCoords, texVert, 2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        shader->disableAttributeArray(BlurTextureShader::Vertices);
        shader->disableAttributeArray(BlurTextureShader::TextureCoords);
        xBlur->release();
    }

    // Render y pass
    QSize ySize(xSize.width(), tr.height() + 2 * steps);
    QGLFramebufferObject *yBlur = acquireFBO(ySize);

    float yWidth = float(ySize.width()) / float(yBlur->width());
    float yHeight = float(ySize.height()) / float(yBlur->height());
    float ystep = radius / float(steps * xBlur->height());
    float yinc = steps / float(xBlur->height());
    {
        yBlur->bind();

        GLSaveViewport sv; GLSaveScissor ss;
        glClearColor(0,0,0,0);
        glDisable(GL_SCISSOR_TEST);
        glViewport(0, 0, yBlur->width(), yBlur->height());
        glClear(GL_COLOR_BUFFER_BIT); 

        float vert[] = { 0, yHeight,
                         yWidth, yHeight,
                         0, 0,
                         yWidth, 0 };
        float texVert[] = { 0, -yinc,
                            xWidth, -yinc,
                            0, xHeight + yinc,
                            xWidth, xHeight + yinc };

        QMatrix4x4 trans;
        trans.translate(-1, -1);
        trans.scale(2, 2);
        BlurTextureShader *shader = item->basicShaders()->blurTexture();
        shader->enable();
        shader->setTransform(trans);
        if (steps > 1) {
            shader->setStep(ystep * 2);
            shader->setSteps(steps / 2);
        } else {
            shader->setStep(ystep);
            shader->setSteps(steps);
        }
        shader->setMode(BlurTextureShader::Vertical);

        glBindTexture(GL_TEXTURE_2D, xBlur->texture());

        shader->setAttributeArray(BlurTextureShader::Vertices, vert, 2);
        shader->setAttributeArray(BlurTextureShader::TextureCoords, texVert, 2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        shader->disableAttributeArray(BlurTextureShader::Vertices);
        shader->disableAttributeArray(BlurTextureShader::TextureCoords);
        yBlur->release();
    }

    glEnable(GL_BLEND);

    // Render display pass
    {
        glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        float vert[] = { -dispSteps, height + dispSteps,
                         width + dispSteps, height + dispSteps,
                         -dispSteps, -dispSteps,
                         width + dispSteps, -dispSteps };
        float texVert[] = { 0, 0,
                            yWidth, 0,
                            0, yHeight,
                            yWidth, yHeight };
        SingleTextureShader *shader = item->basicShaders()->singleTexture();
        shader->enable();
        shader->setTransform(p.activeTransform);

        glBindTexture(GL_TEXTURE_2D, yBlur->texture());

        shader->setAttributeArray(SingleTextureShader::Vertices, vert, 2);
        shader->setAttributeArray(SingleTextureShader::TextureCoords, texVert, 2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        shader->disableAttributeArray(SingleTextureShader::Vertices);
        shader->disableAttributeArray(SingleTextureShader::TextureCoords);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }

    releaseFBO(yBlur);
    releaseFBO(xBlur);
    releaseFBO(fbo);
#else
#if 0
    if (d->radius <= 0) {
        renderToScreen();
        return;
    }
    QSimpleCanvasItem *item = this->item();

    QRect r = item->itemBoundingRect();

    float scale = 0.5;
    float scalePercent = scale / d->radius;
    QGLFramebufferObject *fbo = renderToFBO(scalePercent);
    if (!fbo) 
        return;

    QGLFramebufferObject *xfbo = acquireFBO(QSize(scale * r.width(), fbo->height()));
    QGLFramebufferObject *yfbo = acquireFBO(QSize(scale * r.width(), scale * r.height()));


    BlurTextureShader *shader = item->basicShaders()->blurTexture();
    shader->enable();
    shader->setTransform(QMatrix4x4());

    // Render up - x
    {
        shader->setMode(BlurTextureShader::Horizontal);
        shader->setStep(1. / float(xfbo->width()));

        GLSaveViewport vp;
        xfbo->bind();
        glClearColor(0,0,0,0);
        glViewport(0, 0, xfbo->width(), xfbo->height());
        glClear(GL_COLOR_BUFFER_BIT);

        float oWidth = -1. + 2. * float(r.width()) * scale / float(xfbo->width());
        float oHeight = -1. + 2. * float(r.height()) * scalePercent / float(xfbo->height());
        float vert[] = {
            -1, -1,
            oWidth, -1,
            -1, oHeight,

            -1, oHeight,
            oWidth, oHeight,
            oWidth, -1
        };

        float tWidth = r.width() * scalePercent / fbo->width();
        float tHeight = r.height() * scalePercent / fbo->height();
        float texVert[] = {
            0, 0, 
            tWidth, 0,
            0, tHeight,

            0, tHeight,
            tWidth, tHeight,
            tWidth, 0
        };

        glBindTexture(GL_TEXTURE_2D, fbo->texture());
        shader->setAttributeArray(BlurTextureShader::Vertices, vert, 2);
        shader->setAttributeArray(BlurTextureShader::TextureCoords, texVert, 2);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        xfbo->release();
    }

    // Render up - y
    {
        shader->setMode(BlurTextureShader::Vertical);
        shader->setStep(1. / float(yfbo->height()));

        GLSaveViewport vp;
        yfbo->bind();
        glClearColor(0,0,0,0);
        glViewport(0, 0, yfbo->width(), yfbo->height());
        glClear(GL_COLOR_BUFFER_BIT);

        float oWidth = -1. + 2. * r.width() * scale / yfbo->width();
        float oHeight = -1. + 2. * r.height() * scale / yfbo->height();
        float vert[] = {
            -1, -1,
            oWidth, -1,
            -1, oHeight,

            -1, oHeight,
            oWidth, oHeight,
            oWidth, -1
        };

        float tWidth = r.width() * scale / xfbo->width();
        float tHeight = r.height() * scalePercent / xfbo->height();
        float texVert[] = {
            0, 0, 
            tWidth, 0,
            0, tHeight,

            0, tHeight,
            tWidth, tHeight,
            tWidth, 0
        };

        glBindTexture(GL_TEXTURE_2D, xfbo->texture());
        shader->setAttributeArray(BlurTextureShader::Vertices, vert, 2);
        shader->setAttributeArray(BlurTextureShader::TextureCoords, texVert, 2);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        yfbo->release();
    }

    shader->disableAttributeArray(BlurTextureShader::Vertices);
    shader->disableAttributeArray(BlurTextureShader::TextureCoords);

    float width = r.width();
    float height = r.height();
    //paint to screen
    {
        float texWidth = r.width() * scale / float(yfbo->width());
        float texHeight = r.height() * scale / float(yfbo->height());

        GLfloat vertices[] = { 0, height, 
                               width, height, 
                               0, 0, 
                               width, 0 };
        GLfloat texVertices[] = { 0, 0, 
                                  texWidth, 0, 
                                  0, texHeight,
                                  texWidth, texHeight };

        glBindTexture(GL_TEXTURE_2D, yfbo->texture());

        SingleTextureOpacityShader *shader =
            item->basicShaders()->singleTextureOpacity();
        shader->enable();
        shader->setTransform(p.activeTransform);
        shader->setOpacity(p.activeOpacity);
        shader->setAttributeArray(SingleTextureVertexOpacityShader::Vertices, vertices, 2);
        shader->setAttributeArray(SingleTextureVertexOpacityShader::TextureCoords, texVertices, 2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        shader->disableAttributeArray(SingleTextureVertexOpacityShader::Vertices);
        shader->disableAttributeArray(SingleTextureVertexOpacityShader::TextureCoords);
    }


    releaseFBO(fbo);
    releaseFBO(xfbo);
    releaseFBO(yfbo);
#endif
#endif
#else
    Q_UNUSED(p);
#endif

}

QML_DEFINE_TYPE(QFxBlurFilter,Blur)
QT_END_NAMESPACE
