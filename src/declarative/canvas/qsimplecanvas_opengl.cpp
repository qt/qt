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
#include <glsave.h>
#include <QtOpenGL/qglframebufferobject.h>
#include <gltexture.h>
#include <math.h>


QT_BEGIN_NAMESPACE
void CanvasEGLWidget::paintGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _canvas->paintGL();
}

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

void CanvasEGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

QRect CanvasEGLWidget::map(const QRectF &f) const
{
    return invDefaultTransform.mapRect(f).toAlignedRect();
}

void CanvasEGLWidget::resizeEvent(QResizeEvent *e)
{
    defaultTransform.setIdentity();
    defaultTransform.flipCoordinates();
    defaultTransform.translate(-1, -1, 0);
    defaultTransform.scale(2. / width(), 2. / height(), 1. / (1024. * 1024.));
    invDefaultTransform = defaultTransform.inverted();
    _canvas->root->d_func()->data()->transformActive = defaultTransform;
    _canvas->root->d_func()->data()->transformValid = true;

    QGLWidget::resizeEvent(e);
}

void CanvasEGLWidget::initializeGL()
{
    glEnable(GL_BLEND);
    glEnable(GL_STENCIL_TEST);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 
                        GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glStencilFunc(GL_EQUAL, 0, 0xFFFFFFFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void QSimpleCanvasPrivate::paintGL()
{
    lrpTimer.start();

    QSimpleCanvasItemPrivate::GLPaintParameters p;
    QSimpleCanvasItem::GLPainter painter;

    p.sceneRect = QRect(0, 0, q->width(), q->height());
    p.clipRect = p.sceneRect;
    p.stencilValue = 0;
    p.opacity = 1;
    p.forceParamRefresh = false;
    p.painter = &painter;
    if (!isSetup)  {
        opaqueList = 0;
        int z = 0;
        root->d_func()->setupPainting(0, z, &opaqueList);
    }

    glEnable(GL_DEPTH_TEST);

    glDisable(GL_BLEND);
    painter.blendEnabled = false;
    painter.activeOpacity = 1;
    while (opaqueList) {
        painter.item = opaqueList;
        painter.activeTransform = opaqueList->d_func()->data()->transformActive;
        // ### - I don't think this is right
        painter.sceneClipRect = p.clipRect;

        opaqueList->paintGLContents(painter);
        opaqueList = opaqueList->d_func()->nextOpaque;
    }
    glEnable(GL_BLEND);
    painter.blendEnabled = true;

    root->d_func()->paint(p);

    lrpTime = lrpTimer.elapsed();
}

QGLFramebufferObject *QSimpleCanvasPrivate::acquire(int w, int h)
{
    if (w <= 0 || h <= 0)
        return 0;

    int size = qMax(w, h);
    for (int ii = 0; ii < frameBuffers.count(); ++ii) {
        if (frameBuffers.at(ii)->width() >= size) {
            QGLFramebufferObject *rv = frameBuffers.at(ii);
            frameBuffers.removeAt(ii);
            return rv;
        }
    }

    // Find power of two
    size--;
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    size++;

    QGLFramebufferObject *fbo = new QGLFramebufferObject(size, size);
    return fbo;
}

void QSimpleCanvasPrivate::release(QGLFramebufferObject *buf)
{
    int size = qMax(buf->width(), buf->height());
    for (int ii = 0; ii < frameBuffers.count(); ++ii) {
        if (frameBuffers.at(ii)->width() >= size) {
            frameBuffers.insert(ii, buf);
            return;
        }
    }
    frameBuffers.append(buf);
}

GLBasicShaders *QSimpleCanvasItemPrivate::basicShaders() const
{
    return canvas->d->basicShaders();
}

QSimpleCanvas::Matrix QSimpleCanvasItemPrivate::localTransform() const
{
    Q_Q(const QSimpleCanvasItem);

    QSimpleCanvas::Matrix trans;
    trans.translate(q->x(), q->y());
    QPointF to = transformOrigin();
    trans.translate(to.x(), to.y());
    trans.scale(q->scale(), q->scale());
    trans.translate(-to.x(), -to.y());
    if (data()->transformUser)
        trans *= *data()->transformUser;
    return trans;
}

void QSimpleCanvasItemPrivate::simplePaintChild(const GLPaintParameters &params, QSimpleCanvasItem *child)
{
    Q_Q(QSimpleCanvasItem);

    GLPaintParameters childParams = params;
    if (clip)
        ++childParams.stencilValue;

    if (child->d_func()->data()->activeOpacity != 0) {
        childParams.boundingRect = child->boundingRect();

        if (child->filter() && child->filter()->enabled()) {
            QSimpleCanvasItem::GLPainter &painter = *params.painter;
            painter.item = q;
            painter.activeTransform = child->d_func()->data()->transformActive;
            painter.activeOpacity = child->d_func()->data()->activeOpacity;
            painter.sceneClipRect = params.clipRect;
            child->filter()->d->doFilterGL(painter, childParams);
        } else {
            child->d_func()->paint(childParams);
        }
    }
}

void QSimpleCanvasItemPrivate::paintChild(const GLPaintParameters &params, 
                                       QSimpleCanvasItem *child)
{
    if (params.forceParamRefresh) {
        QSimpleCanvas::Matrix t = child->d_func()->data()->transformActive;
        qreal o = child->d_func()->data()->activeOpacity;
        setupChildState(child);
        simplePaintChild(params, child);
        child->d_func()->data()->transformActive = t;
        child->d_func()->data()->transformValid = true;
        child->d_func()->data()->activeOpacity = o;
    } else {
        simplePaintChild(params, child);
    }
}

void QSimpleCanvasItemPrivate::setupChildState(QSimpleCanvasItem *child)
{
    QSimpleCanvasItemData *const myData = data();
    QSimpleCanvasItemPrivate *const childPrivate = child->d_func();
    QSimpleCanvasItemData *const childData = childPrivate->data();

    childData->activeOpacity = myData->activeOpacity;
    if (childData->visible != 1.)
        childData->activeOpacity *= childData->visible;

    if (childData->activeOpacity != 0.) {
        QSimpleCanvas::Matrix &am = childData->transformActive;
        am = myData->transformActive;

        // Calculate child's transform
        const qreal x = childData->x;
        const qreal y = childData->y;
        const qreal scale = childPrivate->scale;
        QSimpleCanvasItem::Flip flip = childData->flip;

        if (x != 0. || y != 0.)
            am.translate(x, y);

        if (scale != 1.) {
            if (childPrivate->origin == QSimpleCanvasItem::TopLeft) {
                am.scale(scale, scale);
            } else {
                QPointF to = childPrivate->transformOrigin();
                am.translate(to.x(), to.y());
                am.scale(scale, scale);
                am.translate(-to.x(), -to.y());
            }
        }

        if (childData->transformUser)
            am *= *childData->transformUser;
        if (flip) {
            QRectF br = child->boundingRect();
            am.translate(br.width() / 2., br.height() / 2);
            am.rotate(180, (flip & QSimpleCanvasItem::VerticalFlip)?1:0, (flip & QSimpleCanvasItem::HorizontalFlip)?1:0, 0);
            am.translate(-br.width() / 2., -br.height() / 2);
        }

        childData->transformValid = true;
    } 
}

#define QSIMPLECANVAS_DISABLE_TREE_CLIPPING
QRectF QSimpleCanvasItemPrivate::setupPainting(int version, int &z, QSimpleCanvasItem **opaqueList)
{
    static QRectF scene(-1., -1., 2., 2.);
    Q_Q(QSimpleCanvasItem);

    bool hasContents = options & QSimpleCanvasItem::HasContents;

    QSimpleCanvasItemData *myData = data();

    QRectF rv;
    if (hasContents) {
        QRectF filteredBoundRect = q->boundingRect();
        if (filter)
            filteredBoundRect = filter->itemBoundingRect(filteredBoundRect);
        const QMatrix4x4 &active = myData->transformActive;

        rv = active.mapRect(filteredBoundRect);
    } 
#ifdef QSIMPLECANVAS_DISABLE_TREE_CLIPPING
    myData->doNotPaint = false;
    myData->doNotPaintChildren = false;
#else
    myData->doNotPaint = !hasContents || !rv.intersects(scene);
    myData->doNotPaintChildren = hasContents && myData->doNotPaint && 
                                 (clip != QSimpleCanvasItem::NoClip);
#endif

    if (!myData->doNotPaint &&
        (options & QSimpleCanvasItem::IsOpaque) && 
        (myData->activeOpacity == 1.) && 
        (clip == 0)) {

        nextOpaque = *opaqueList;
        *opaqueList = q;
        myData->doNotPaint = true;

    }

    int myZ = z++;

    if (myData->doNotPaintChildren) {
        rv = QRectF();
    } else {
        zOrderChildren();

        for (int ii = 0; ii < children.count(); ++ii) {
            QSimpleCanvasItem *child = children.at(ii);
            setupChildState(child);

            QSimpleCanvasItemData *childData = child->d_func()->data();
            if (childData->activeOpacity != 0) 
                rv |= child->d_func()->setupPainting(version, z, opaqueList);
        } 

#ifndef QSIMPLECANVAS_DISABLE_TREE_CLIPPING
        myData->doNotPaintChildren |= !rv.intersects(scene);
#endif
    }

    myData->transformActive.translate(0, 0, myZ);

    myData->lastPaintRect = rv;

    return rv;
}


void QSimpleCanvasItemPrivate::paint(GLPaintParameters &oldParams, QSimpleCanvasFilter::Layer layer)
{
    if (!layer)
        return;

    Q_Q(QSimpleCanvasItem);

    bool doNotPaintChildren = data()->doNotPaintChildren;
    if (doNotPaintChildren) 
        return;

    GLPaintParameters params = oldParams;

    qreal width = params.boundingRect.width();
    qreal height = params.boundingRect.height();

    GLfloat vertices[] = { 0, height,
                           width, height,
                           0, 0,
                           width, 0 };

    // XXX Handle separate cliping modes
    if (clip) {
        if (params.stencilValue == 255) 
            qWarning() 
                << "OpenGL: Clip recursion greater than 255 not permitted.";
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

        glStencilFunc(GL_EQUAL, params.stencilValue, 0xFFFFFFFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

        ConstantColorShader *shader = basicShaders()->constantColor();
        params.painter->invalidate();
        shader->enable();
        shader->setTransform(data()->transformActive);

        shader->setAttributeArray(ConstantColorShader::Vertices, vertices, 2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glStencilFunc(GL_EQUAL, params.stencilValue + 1, 0xFFFFFFFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        // XXX Copied from qsimplecanvas_opengl1 scissor based clipping
        QRectF r = data()->transformActive.mapRect(params.boundingRect);
        r.translate(1, 1);
        float xscale = 0.5 * float(params.sceneRect.width());
        float yscale = 0.5 * float(params.sceneRect.height());
        r.moveTo(r.x() * xscale, r.y() * yscale);
        r.setSize(QSizeF(r.width() * xscale, r.height() * yscale));
        int sr_x = int(::floorf(r.x()));
        int sr_y = int(::floorf(r.y()));
        int sr_width = int(::ceilf(r.right())) - sr_x;
        int sr_height = int(::ceilf(r.bottom())) - sr_y;
        QRect sr(sr_x, sr_y, sr_width, sr_height);
        sr.moveTo(sr.x(), params.sceneRect.height() - sr.y() - sr.height());
        sr &= params.clipRect;
        params.clipRect = sr;
    }

    int upto = 0;
    for (upto = 0; upto < children.count(); ++upto) {
        QSimpleCanvasItem *c = children.at(upto);
        if (c->z() < 0) {
            if (layer & QSimpleCanvasFilter::ChildrenUnderItem) 
                paintChild(params, c);
        } else {
            break;
        }
    }

    bool doNotPaint = data()->doNotPaint;

    if (!doNotPaint && layer & QSimpleCanvasFilter::Item) {
        QSimpleCanvasItem::GLPainter &painter = *params.painter;
        painter.item = q;
        painter.activeTransform = data()->transformActive;
        painter.activeOpacity = data()->activeOpacity;
        painter.sceneClipRect = params.clipRect;

        q->paintGLContents(painter);
    }

    if (layer & QSimpleCanvasFilter::ChildrenAboveItem) {
        for (; upto < children.count(); ++upto) {
            QSimpleCanvasItem *c = children.at(upto);
            paintChild(params, c);
        }
    }

    if (clip) {
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glStencilFunc(GL_EQUAL, params.stencilValue + 1, 0xFFFFFFFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
 
        params.painter->invalidate();
        ConstantColorShader *shader = basicShaders()->constantColor();
        shader->enable();
        shader->setTransform(data()->transformActive);

        shader->setAttributeArray(ConstantColorShader::Vertices, vertices, 2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glStencilFunc(GL_EQUAL, params.stencilValue, 0xFFFFFFFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
}

enum ShaderType { ST_None, ST_SingleTexture, ST_SingleTextureOpacity, ST_Color };

QSimpleCanvasItem::GLPainter::GLPainter() 
: item(0), activeOpacity(1), blendEnabled(true), flags(0)
{
}

QSimpleCanvasItem::GLPainter::GLPainter(QSimpleCanvasItem *i) 
: item(i), activeOpacity(1), blendEnabled(true), flags(0) 
{
}

QGLShaderProgram *QSimpleCanvasItem::GLPainter::useTextureShader()
{
    if (activeOpacity == 1.) {
        SingleTextureShader *shader = item->basicShaders()->singleTexture();
        if (flags != ST_SingleTexture) {
            shader->enable();
            flags = ST_SingleTexture;
        }

        shader->setTransform(activeTransform);
        return shader;
    } else {
        SingleTextureOpacityShader *shader = item->basicShaders()->singleTextureOpacity();

        if (flags != ST_SingleTextureOpacity) {
            shader->enable();
            flags = ST_SingleTextureOpacity;
        }

        shader->setTransform(activeTransform);
        shader->setOpacity(activeOpacity);
        return shader;
    }
}

void QSimpleCanvasItem::GLPainter::invalidate()
{
    flags = ST_None;
}

QGLShaderProgram *QSimpleCanvasItem::GLPainter::useColorShader(const QColor &color)
{
    QColor c = color;

    ConstantColorShader *shader = item->basicShaders()->constantColor();

    if (flags != ST_Color) {
        shader->enable();
        flags = ST_Color;
    }

    if (activeOpacity != 1.) 
        c.setAlpha(int(c.alpha() * activeOpacity));

    shader->setColor(c);
    shader->setTransform(activeTransform);

    return shader;
}

void QSimpleCanvasItem::GLPainter::drawPixmap(const QPointF &point, 
                                              const GLTexture &texture)
{
    drawPixmap(QRectF(point, QSizeF(texture.width(), texture.height())), texture);
}

void QSimpleCanvasItem::GLPainter::drawPixmap(const QRectF &rect, 
                                              const GLTexture &img)
{
    QGLShaderProgram *shader = useTextureShader();

    GLfloat vertices[8];
    GLfloat texVertices[8];

    float widthV = img.width();
    float heightV = img.height();

    vertices[0] = rect.x(); vertices[1] = rect.y()+heightV;
    vertices[2] = rect.x()+widthV; vertices[3] = rect.y()+heightV;
    vertices[4] = rect.x(); vertices[5] = rect.y();
    vertices[6] = rect.x()+widthV; vertices[7] = rect.y();

    texVertices[0] = 0; texVertices[1] = 0;
    texVertices[2] = 1; texVertices[3] = 0;
    texVertices[4] = 0; texVertices[5] = 1;
    texVertices[6] = 1; texVertices[7] = 1;

    shader->setAttributeArray(SingleTextureShader::Vertices, vertices, 2);
    shader->setAttributeArray(SingleTextureShader::TextureCoords, texVertices, 2);

    glBindTexture(GL_TEXTURE_2D, img.texture());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void QSimpleCanvasItem::GLPainter::fillRect(const QRectF &rect, 
                                            const QColor &color)
{
    if (color.alpha() == 0xFF) 
        glDisable(GL_BLEND);

    QGLShaderProgram *shader = useColorShader(color);
    float x = rect.x();
    float y = rect.y();
    float width = rect.width();
    float height = rect.height();

    GLfloat vertices[] = { x, height,
                           width, height,
                           x, y,

                           width, height,
                           x, y,
                           width, y };

    shader->setAttributeArray(ConstantColorShader::Vertices, vertices, 2);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    if (color.alpha() == 0xFF)
        glEnable(GL_BLEND);
}

void QSimpleCanvasItem::CachedTexture::addRef()
{
    ++r;
}

void QSimpleCanvasItem::CachedTexture::release()
{
    Q_ASSERT(r > 0);
    --r;

    if (r == 0) {
        if (!s.isEmpty())
            d->cachedTextures.remove(s);
        delete this;
    }
}

int QSimpleCanvasItem::CachedTexture::pixmapWidth() const
{
    return w;
}

int QSimpleCanvasItem::CachedTexture::pixmapHeight() const
{
    return h;
}

QSimpleCanvasItem::CachedTexture::CachedTexture()
: r(0), w(0), h(0)
{
}

QSimpleCanvasItem::CachedTexture *
QSimpleCanvasItem::cachedTexture(const QString &key)
{
    Q_D(QSimpleCanvasItem); 
    if (!d->canvas || key.isEmpty())
        return 0;

    QSimpleCanvasPrivate *canvas = d->canvas->d;
    QHash<QString, QSimpleCanvasItem::CachedTexture *>::ConstIterator iter = 
        canvas->cachedTextures.find(key);
    if (iter != canvas->cachedTextures.end()) {
        (*iter)->addRef();
        return (*iter);
    } else {
        return 0;
    }
}

QSimpleCanvasItem::CachedTexture *
QSimpleCanvasItem::cachedTexture(const QString &key, const QPixmap &pix)
{
    Q_D(QSimpleCanvasItem); 
    if (!d->canvas)
        return 0;

    QSimpleCanvasPrivate *canvas = d->canvas->d;
    QHash<QString, QSimpleCanvasItem::CachedTexture *>::ConstIterator iter = 
        canvas->cachedTextures.end();
    if (!key.isEmpty())
        iter = canvas->cachedTextures.find(key);

    if (iter != canvas->cachedTextures.end()) {
        (*iter)->addRef();
        return (*iter);
    } else {
        CachedTexture *rv = new CachedTexture;
        rv->s = key;
        rv->d = canvas;
        rv->w = pix.width();
        rv->h = pix.height();
        rv->setImage(pix.toImage(), GLTexture::PowerOfTwo);
        rv->setHorizontalWrap(GLTexture::Repeat);
        rv->setVerticalWrap(GLTexture::Repeat);
        rv->addRef();
        if (!key.isEmpty())
            canvas->cachedTextures.insert(key, rv);
        return rv;
    }
}

QT_END_NAMESPACE
