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

#include "qsimplecanvasfilter.h"
#include "qsimplecanvasfilter_p.h"
#include "qsimplecanvasitem_p.h"
#include "qsimplecanvas.h"
#include "qsimplecanvas_p.h"

#if defined(QFX_RENDER_OPENGL2)
#include <glsave.h>
#include <QtOpenGL/qglframebufferobject.h>
#endif

QT_BEGIN_NAMESPACE

/*!
    \qmlclass Filter
    \brief A Filter is a graphical filter that can be applied to items. 
*/

QSimpleCanvasFilter::QSimpleCanvasFilter(QObject *parent)
: QObject(parent), d(new QSimpleCanvasFilterPrivate(this))
{
}

QSimpleCanvasFilter::~QSimpleCanvasFilter()
{
    if(d->item) d->item->setFilter(0);
    delete d; d = 0;
}

bool QSimpleCanvasFilter::enabled() const
{
    return d->enabled;
}

/*!
    \qmlproperty bool Filter::enabled
    \brief Controls whether the filter is applied.
*/

void QSimpleCanvasFilter::setEnabled(bool e) 
{
    if(e == d->enabled)
        return;
    d->enabled = e;
    emit enabledChanged();
    update();
}

QGLFramebufferObject *QSimpleCanvasFilter::renderToFBO(float scale, const QRectF &src, const QPoint &offset, Layer)
{
    // XXX - respect src
#if defined(QFX_RENDER_OPENGL2)
    Q_UNUSED(src);
    Q_UNUSED(offset);

    QSimpleCanvasItem *item = d->item;
    QRect br = item->itemBoundingRect();
    if(br.isEmpty())
        return 0;
    QGLFramebufferObject *fbo = 
        item->canvas()->d->acquire(int(br.width() * scale), int(br.height() * scale));

    GLSaveViewport sv; GLSaveScissor ss;
    qreal oldOpacity = item->d_func()->data()->activeOpacity;
    item->d_func()->data()->activeOpacity = 1;

    fbo->bind();

    glClearColor(0,0,0,0);
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, int(br.width() * scale), int(br.height() * scale));
    glClear(GL_COLOR_BUFFER_BIT); 
    glViewport(0, 0, int(br.width() * scale), int(br.height() * scale));

    QMatrix4x4 mat;
    mat.flipCoordinates();
    mat.translate(-1, -1, 0);
    mat.scale(2. / (br.width()), 2. / (br.height()), 1. / (1024. * 1024.));

    renderToScreen(mat);

    fbo->release();

    item->d_func()->data()->activeOpacity = oldOpacity;
    return fbo;
#else
    Q_UNUSED(src);
    Q_UNUSED(offset);
    Q_UNUSED(scale);
    return 0;
#endif
}

QGLFramebufferObject *QSimpleCanvasFilter::renderToFBO(const QRectF &src, const QPoint &offset, Layer)
{
    // XXX - respect src
#if defined(QFX_RENDER_OPENGL2)
    Q_UNUSED(src);
    Q_UNUSED(offset);

    QSimpleCanvasItem *item = d->item;
    QRect br = item->itemBoundingRect();
    if(br.isEmpty())
        return 0;
    QGLFramebufferObject *fbo = 
        item->canvas()->d->acquire(br.width(), br.height());

    GLSaveViewport sv; GLSaveScissor ss;
    qreal oldOpacity = item->d_func()->data()->activeOpacity;
    item->d_func()->data()->activeOpacity = 1;

    fbo->bind();

    glClearColor(0,0,0,0);
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, br.width(), br.height());
    glClear(GL_COLOR_BUFFER_BIT); 
    glViewport(0, 0, br.width(), br.height());

    QMatrix4x4 mat;
    mat.flipCoordinates();
    mat.translate(-1, -1, 0);
    mat.scale(2. / br.width(), 2. / br.height(), 1. / (1024. * 1024.));

    renderToScreen(mat);

    fbo->release();

    item->d_func()->data()->activeOpacity = oldOpacity;
    return fbo;
#else
    Q_UNUSED(src);
    Q_UNUSED(offset);
    return 0;
#endif
}

QGLFramebufferObject *QSimpleCanvasFilter::acquireFBO(const QSize &s)
{
#if defined(QFX_RENDER_OPENGL2)
    QSize size;
    QSimpleCanvasItem *item = d->item;
    if(size.isNull()) {
        QRect br = item->itemBoundingRect();
        size = br.size();
    } else {
        size = s;
    }

    QGLFramebufferObject *fbo = 
        item->canvas()->d->acquire(s.width(), s.height());

    return fbo;
#else
    Q_UNUSED(s);
    return 0;
#endif
}

void QSimpleCanvasFilter::releaseFBO(QGLFramebufferObject *fbo)
{
#if defined(QFX_RENDER_OPENGL2)
    d->item->d_func()->canvas->d->release(fbo);
#else
    Q_UNUSED(fbo);
#endif
}

void QSimpleCanvasFilter::renderToScreen(const QRectF &src, Layer layer)
{
    // XXX - respect src
#if defined(QFX_RENDER_OPENGL2)
    Q_UNUSED(src);
    Q_UNUSED(layer);
    d->item->d_func()->paint(d->params, layer);
#else
    Q_UNUSED(src);
    Q_UNUSED(layer);
#endif
}

void QSimpleCanvasFilter::renderToScreen(const QSimpleCanvas::Matrix &trans, const QRectF &src, Layer layer)
{
    // XXX - respect src
#if defined(QFX_RENDER_OPENGL2)
    Q_UNUSED(src);
    QSimpleCanvas::Matrix old = d->item->d_func()->data()->transformActive;
    d->item->d_func()->data()->transformActive = trans;
    QSimpleCanvasItemPrivate::GLPaintParameters params = d->params;
    params.forceParamRefresh = true;
    d->item->d_func()->paint(params, layer);
    d->item->d_func()->data()->transformActive = old;
#else
    Q_UNUSED(trans);
    Q_UNUSED(src);
    Q_UNUSED(layer);
#endif
}

QSimpleCanvasItem *QSimpleCanvasFilter::item() const
{
    return d->item;
}

void QSimpleCanvasFilter::setItem(QSimpleCanvasItem *i)
{
    if(d->item == i)
        return;
    if(d->item) {
        d->item->setFilter(0);
        d->item = 0;
    }

    if(i->filter() != this) {
        i->setFilter(this);
    } else {
        d->item = i;
    }
}

#if defined(QFX_RENDER_OPENGL2)
void QSimpleCanvasFilterPrivate::doFilterGL(QSimpleCanvasItem::GLPainter &p, const QSimpleCanvasItemPrivate::GLPaintParameters &prms)
{
    params = prms;
    q->filterGL(p);
}
#endif

QRectF QSimpleCanvasFilter::itemBoundingRect(const QRectF &r) const
{
    return r;
}

void QSimpleCanvasFilter::filterGL(QSimpleCanvasItem::GLPainter &p)
{
    Q_UNUSED(p);
}

void QSimpleCanvasFilter::filter(QPainter &)
{
}

void QSimpleCanvasFilter::update()
{
    if(d->item)
        d->item->update();
}

#if defined(QFX_RENDER_OPENGL2)
bool QSimpleCanvasFilterPrivate::isSimpleItem(QSimpleCanvasItem *item, QSimpleCanvasItem **out, QSimpleCanvas::Matrix *mout)
{
    if(item->options() & QSimpleCanvasItem::SimpleItem && !item->hasChildren()) {
        *out = item;
        return true;
    } else if(!(item->options() & QSimpleCanvasItem::HasContents) && 
                item->children().count() == 1) {
        QSimpleCanvasItem *child = item->children().first();
        if(child->filter() && child->filter()->enabled())
            return false;
        bool rv = isSimpleItem(child, out, mout);
        if(rv)
            *mout *= child->d_func()->localTransform();
        return rv;
    } else {
        return false;
    }
}
#endif

bool QSimpleCanvasFilter::isSimpleItem(QSimpleCanvasItem **out, QSimpleCanvas::Matrix *mout)
{
#if defined(QFX_RENDER_OPENGL2)
    return d->isSimpleItem(item(), out, mout);
#else
    Q_UNUSED(out);
    Q_UNUSED(mout);
#endif

    return false;
}

QT_END_NAMESPACE
