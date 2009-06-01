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

#include "qsimplecanvas.h"
#include "qsimplecanvas_p.h"
#include "qsimplecanvasitem_p.h"
#include "qsimplecanvasfilter_p.h"
#include <gltexture.h>
#include <glsave.h>
#include <math.h>


QT_BEGIN_NAMESPACE
void CanvasEGLWidget::updateGL()
{
    _clip = QRect();
    QGLWidget::updateGL();
}

void CanvasEGLWidget::updateGL(const QRect &r)
{
    if (r.isEmpty())
        return;

    _clip = r;
    QGLWidget::updateGL();
}

void CanvasEGLWidget::paintGL()
{
    if (!_clip.isEmpty()) {
        glEnable(GL_SCISSOR_TEST);
        glScissor(_clip.x(), _clip.y(), _clip.width(), _clip.height());
    } else {
        glDisable(GL_SCISSOR_TEST);
    }

    glDepthMask(GL_TRUE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthMask(GL_FALSE);

    _canvas->paintGL();
}

QRect CanvasEGLWidget::map(const QRectF &f)
{
    return invDefaultTransform.mapRect(f).toAlignedRect();
}

void CanvasEGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void CanvasEGLWidget::resizeEvent(QResizeEvent *e)
{
    defaultTransform.identity();
    defaultTransform.flipCoordinates();
    defaultTransform.translate(-1, -1, 0);
    defaultTransform.scale(2. / width(), 2. / height(), -2. / 65536.);
    invDefaultTransform = defaultTransform.inverted();
    _canvas->root->d_func()->transformActive = defaultTransform;
    QGLWidget::resizeEvent(e);
}

void CanvasEGLWidget::initializeGL()
{
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_GREATER);
#ifdef QT_OPENGL_ES
    glClearDepthf(0);
#else
    glClearDepth(0);
#endif
}

void QSimpleCanvasPrivate::paintGL()
{
    lrpTimer.start();

    QSimpleCanvasItemPrivate::GLPaintParameters p;
    p.sceneRect = QRect(0, 0, q->width(), q->height());
    p.clipRect = p.sceneRect;
    p.opacity = 1;
    p.forceParamRefresh = false;
    if (!isSetup) {
        unsigned int zero = 0;
        root->d_func()->setupPainting(0, QRect(), &zero);
    }
    root->d_func()->paint(p);

    lrpTime = lrpTimer.elapsed();
}

QSimpleCanvas::Matrix QSimpleCanvasItemPrivate::localTransform() const
{
    Q_Q(const QSimpleCanvasItem);

    QSimpleCanvas::Matrix trans;
    trans.translate(q->x(), q->y());
    QPointF to = transformOrigin();
    trans.translate(to.x(), to.y());
    trans.scale(q->scale().value(), q->scale().value());
    trans.translate(-to.x(), -to.y());
    trans *= transformUser;
    return trans;
}

void QSimpleCanvasItemPrivate::simplePaintChild(const GLPaintParameters &params, QSimpleCanvasItem *child)
{
    GLPaintParameters childParams = params;

    if (child->d_func()->activeOpacity != 0) {
        childParams.boundingRect = child->boundingRect();
        child->d_func()->paint(childParams);
    }
}

void QSimpleCanvasItemPrivate::paintChild(const GLPaintParameters &params, 
                                       QSimpleCanvasItem *child)
{
    if (params.forceParamRefresh) {
        QSimpleCanvas::Matrix t = child->d_func()->transformActive;
        qreal o = child->d_func()->activeOpacity;
        setupChildState(child);
        simplePaintChild(params, child);
        child->d_func()->transformActive = t;
        child->d_func()->activeOpacity = o;
    } else {
        simplePaintChild(params, child);
    }
}


void QSimpleCanvasItemPrivate::setupChildState(QSimpleCanvasItem *child)
{
    qreal visible = child->visible().value();
    child->d_func()->activeOpacity = activeOpacity;
    if (visible != 1)
        child->d_func()->activeOpacity *= visible;

    if (child->d_func()->activeOpacity != 0) {
        // Calculate child's transform
        qreal x = child->x();
        qreal y = child->y();
        qreal scale = child->scale().value();
        QSimpleCanvasItem::Flip flip = child->flip();

        QSimpleCanvas::Matrix &am = child->d_func()->transformActive;
        am = transformActive;
        if (x != 0 || y != 0)
            am.translate(x, y);
        if (scale != 1) {
            QPointF to = child->d_func()->transformOrigin();
            if (to.x() != 0. || to.y() != 0.)
                am.translate(to.x(), to.y());
            am.scale(scale, scale);
            if (to.x() != 0. || to.y() != 0.)
                am.translate(-to.x(), -to.y());
        }
        if (child->d_func()->transformUserSet)
            am *= child->d_func()->transformUser;
        if (flip) {
            QRectF br = child->boundingRect();
            am.translate(br.width() / 2., br.height() / 2);
            am.rotate(180, (flip & QSimpleCanvasItem::VerticalFlip)?1:0, (flip & QSimpleCanvasItem::HorizontalFlip)?1:0, 0);
            am.translate(-br.width() / 2., -br.height() / 2);
        }
    } 
}

QRectF QSimpleCanvasItemPrivate::setupPainting(int version, const QRect &bounding, unsigned int *zero)
{
    Q_Q(QSimpleCanvasItem);

    QRectF rv = transformActive.mapRect(q->boundingRect());

    unsigned int oldZero = *zero;

    for (int ii = 0; ii < children.count(); ++ii) {
        QSimpleCanvasItem *child = children.at(ii);
        setupChildState(child);

        if (child->d_func()->activeOpacity != 0) 
            rv |= child->d_func()->setupPainting(version, bounding, zero);
    } 

    if (clip || oldZero != *zero)
        (*zero)++;
    transformActive.translate(0, 0, *zero);

    lastPaintRect = rv;
    return rv;
}

void QSimpleCanvasItemPrivate::paintNoClip(GLPaintParameters &params, QSimpleCanvasFilter::Layer layer)
{
    Q_Q(QSimpleCanvasItem);

    zOrderChildren();

    int upto = 0;
    for (upto = 0; upto < children.count(); ++upto) {
        QSimpleCanvasItem *c = children.at(upto);
        if (c->zValue().value() < 0) {
            if (layer & QSimpleCanvasFilter::ChildrenUnderItem) 
                paintChild(params, c);
        } else {
            break;
        }
    }

    if (layer & QSimpleCanvasFilter::Item && 
       q->options() & QSimpleCanvasItem::HasContents) {
        QSimpleCanvasItem::GLPainter painter(q);
        painter.activeTransform = transformActive;
        painter.activeOpacity = activeOpacity;
        painter.sceneClipRect = params.clipRect;

        q->paintGLContents(painter);
    }

    if (layer & QSimpleCanvasFilter::ChildrenAboveItem) {
        for (; upto < children.count(); ++upto) {
            QSimpleCanvasItem *c = children.at(upto);
            paintChild(params, c);
        }
    }
}

void QSimpleCanvasItemPrivate::paint(GLPaintParameters &params, QSimpleCanvasFilter::Layer layer)
{
    if (!layer)
        return;

    // XXX Handle separate cliping modes
    if (clip) {

        GLSaveScissor ss;
        qreal width = params.boundingRect.width();
        qreal height = params.boundingRect.height();
        float margin = width + height;

        GLfloat clipvertices[] = 
        { 
            -margin, -margin,
            margin, -margin,
            margin, 0,

            -margin, -margin,
            -margin, 0,
            margin, 0,

            -margin, 0, 
            -margin, margin,
            0, 0,

            0, 0,
            0, margin,
            -margin, margin,

            0, height, 
            0, margin,
            margin, margin,

            margin, margin,
            0, height, 
            margin, height,

            width, 0, 
            margin, 0,
            margin, height,

            margin, height,
            width, height,
            width, 0
        };

        QRectF r = transformActive.mapRect(params.boundingRect);
        r.translate(1, 1);
        float xscale = 0.5 * float(params.sceneRect.width());
        float yscale = 0.5 * float(params.sceneRect.height());
        r.moveTo(r.x() * xscale, r.y() * yscale);
        r.setSize(QSizeF(r.width() * xscale, r.height() * yscale));

        glEnable(GL_SCISSOR_TEST);
        int sr_x = ::floorf(r.x());
        int sr_y = ::floorf(r.y());
        int sr_width = ::ceilf(r.right()) - sr_x;
        int sr_height = ::ceilf(r.bottom()) - sr_y;

        QRect sr(sr_x, sr_y, sr_width, sr_height);
        if (ss.wasEnabled())
            sr &= ss.rect();

        glScissor(sr.x(), sr.y(), sr.width(), sr.height());

        {
            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(transformActive.data());
            glDepthMask(GL_TRUE);
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(2, GL_FLOAT, 0, clipvertices);
            glDrawArrays(GL_TRIANGLES, 0, 24);
            glDisableClientState(GL_VERTEX_ARRAY);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glDepthMask(GL_FALSE);
        }

        GLPaintParameters newParams = params;
        newParams.clipRect = sr;
        newParams.clipRect.moveTo(sr.x(), params.sceneRect.height() - sr.y() - sr.height());
        paintNoClip(newParams, layer);
    } else {
        paintNoClip(params, layer);
    }
}

QGLShaderProgram *QSimpleCanvasItem::GLPainter::useTextureShader()
{
    qFatal("Cannot call QSimpleCanvasItem::GLPainter::useTextureShader() when using OpenGL ES 1.1");
    return 0;
}

QGLShaderProgram *QSimpleCanvasItem::GLPainter::useColorShader(const QColor &color)
{
    Q_UNUSED(color);
    qFatal("Cannot call QSimpleCanvasItem::GLPainter::useColorShader() when using OpenGL ES 1.1");
    return 0;
}

GLBasicShaders *QSimpleCanvasItemPrivate::basicShaders() const
{
    qFatal("Cannot call QSimpleCanvasItem::basicShaders() when using OpenGL ES 1.1");
    return 0;
}

QGLFramebufferObject *QSimpleCanvasPrivate::acquire(int, int)
{
    return 0;
}

void QSimpleCanvasPrivate::release(QGLFramebufferObject *)
{
}

void  QSimpleCanvasItem::GLPainter::drawPixmap(const QPointF &point, 
                                          const GLTexture &texture)
{
    drawPixmap(QRectF(point, QSizeF(texture.width(), texture.height())), texture);
}

void  QSimpleCanvasItem::GLPainter::drawPixmap(const QRectF &rect, 
                                          const GLTexture &img)
{
    qFatal("Cannot call QSimpleCanvasItem::GLPainter::drawPixmap() when using OpenGL ES 1.1");
}

QT_END_NAMESPACE
