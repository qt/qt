/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsgpainteditem.h"
#include <private/qsgpainteditem_p.h>
#include <private/qsgpainternode_p.h>

#include <private/qsgcontext_p.h>
#include <private/qsgadaptationlayer_p.h>

QT_BEGIN_NAMESPACE

/*!
    \class QSGPaintedItem
    \brief The QSGPaintedItem class provides a way to use the QPainter API in the
    QML Scene Graph.

    The QSGPaintedItem makes it possible to use the QPainter API with the QML Scene Graph.
    It sets up a textured rectangle in the Scene Graph and uses a QPainter to paint
    onto the texture. The render target can be either a QImage or a QGLFramebufferObject.
    When the render target is a QImage, QPainter first renders into the image then
    the content is uploaded to the texture.
    When a QGLFramebufferObject is used, QPainter paints directly onto the texture.
    Call update() to trigger a repaint.

    Set the \l smooth property to true to enable QPainter to do anti-aliased rendering.

    QSGPaintedItem is meant to make it easier to port old code that is using the
    QPainter API to the QML Scene Graph API and it should be used only for that purpose.

    To write your own painted item, you first create a subclass of QSGPaintedItem, and then
    start by implementing its only pure virtual public function: paint(), which implements
    the actual painting. To get the size of the area painted by the item, use
    QSGItem::width() and QSGItem::height().
*/

/*!
    \enum QSGPaintedItem::RenderTarget

    This enum describes QSGPaintedItem's render targets. The render target is the
    surface QPainter paints onto before the item is rendered on screen.

    \value Image The default; QPainter paints into a QImage using the raster paint engine.
    The image's content needs to be uploaded to graphics memory afterward, this operation
    can potentially be slow if the item is large. This render target allows high quality
    anti-aliasing and fast item resizing.

    \value FramebufferObject QPainter paints into a QGLFramebufferObject using the GL
    paint engine. Painting can be faster as no texture upload is required, but anti-aliasing
    quality is not as good as if using an image. This render target allows faster rendering
    in some cases, but you should avoid using it if the item is resized often.

    \sa setRenderTarget()
*/

/*!
    \internal
*/
QSGPaintedItemPrivate::QSGPaintedItemPrivate()
    : QSGItemPrivate()
    , fillColor(Qt::transparent)
    , renderTarget(QSGPaintedItem::Image)
    , geometryDirty(false)
    , contentsDirty(false)
    , opaquePainting(false)
{
}

/*!
    Constructs a QSGPaintedItem with the given \a parent item.
 */
QSGPaintedItem::QSGPaintedItem(QSGItem *parent)
    : QSGItem(*(new QSGPaintedItemPrivate), parent)
{
    setFlag(ItemHasContents);
}

/*!
    \internal
*/
QSGPaintedItem::QSGPaintedItem(QSGPaintedItemPrivate &dd, QSGItem *parent)
    : QSGItem(dd, parent)
{
    setFlag(ItemHasContents);
}

/*!
    Destroys the QSGPaintedItem.
*/
QSGPaintedItem::~QSGPaintedItem()
{
}

/*!
    Schedules a redraw of the area covered by \a rect in this item. You can call this function
    whenever your item needs to be redrawn, such as if it changes appearance or size.

    This function does not cause an immediate paint; instead it schedules a paint request that
    is processed by the QML Scene Graph when the next frame is rendered. The item will only be
    redrawn if it is visible.

    Note that calling this function will trigger a repaint of the whole scene.

    \sa paint()
*/
void QSGPaintedItem::update(const QRect &rect)
{
    Q_D(QSGPaintedItem);
    d->contentsDirty = true;
    if (rect.isNull() && !d->dirtyRect.isNull())
        d->dirtyRect = boundingRect().toAlignedRect();
    else
        d->dirtyRect |= (boundingRect() & rect).toAlignedRect();
    QSGItem::update();
}

/*!
    Returns true if this item is opaque; otherwise, false is returned.

    By default, painted items are not opaque.

    \sa setOpaquePainting()
*/
bool QSGPaintedItem::opaquePainting() const
{
    Q_D(const QSGPaintedItem);
    return d->opaquePainting;
}

/*!
    If \a opaque is true, the item is opaque; otherwise, it is considered as translucent.

    Opaque items are not blended with the rest of the scene, you should set this to true
    if the content of the item is opaque to speed up rendering.

    By default, painted items are not opaque.

    \sa opaquePainting()
*/
void QSGPaintedItem::setOpaquePainting(bool opaque)
{
    Q_D(QSGPaintedItem);

    if (d->opaquePainting == opaque)
        return;

    d->opaquePainting = opaque;
    QSGItem::update();
}

QSize QSGPaintedItem::contentsSize() const
{
    // XXX todo
    return QSize();
}

void QSGPaintedItem::setContentsSize(const QSize &)
{
    // XXX todo
}

void QSGPaintedItem::resetContentsSize()
{
    // XXX todo
}

qreal QSGPaintedItem::contentsScale() const
{
    // XXX todo
    return 1;
}

void QSGPaintedItem::setContentsScale(qreal)
{
    // XXX todo
}

int QSGPaintedItem::pixelCacheSize() const
{
    // XXX todo
    return 0;
}

void QSGPaintedItem::setPixelCacheSize(int)
{
    // XXX todo
}

bool QSGPaintedItem::smoothCache() const
{
    // XXX todo
    return false;
}

void QSGPaintedItem::setSmoothCache(bool)
{
    // XXX todo
}

/*!
    \property QSGPaintedItem::fillColor
    \brief The item's background fill color.

    By default, the fill color is set to Qt::transparent.
*/
QColor QSGPaintedItem::fillColor() const
{
    Q_D(const QSGPaintedItem);
    return d->fillColor;
}

void QSGPaintedItem::setFillColor(const QColor &c)
{
    Q_D(QSGPaintedItem);

    if (d->fillColor == c)
        return;

    d->fillColor = c;
    update();

    emit fillColorChanged();
}

/*!
    \property QSGPaintedItem::renderTarget
    \brief The item's render target.

    This property defines which render target the QPainter renders into, it can be either
    QSGPaintedItem::Image or QSGPaintedItem::FramebufferObject. Both have certains benefits,
    typically performance versus quality. Using a framebuffer object avoids a costly upload
    of the image contents to the texture in graphics memory, while using an image enables
    high quality anti-aliasing.

    \warning Resizing a framebuffer object is a costly operation, avoid using
    the QSGPaintedItem::FramebufferObject render target if the item gets resized often.

    By default, the render target is QSGPaintedItem::Image.
*/
QSGPaintedItem::RenderTarget QSGPaintedItem::renderTarget() const
{
    Q_D(const QSGPaintedItem);
    return d->renderTarget;
}

void QSGPaintedItem::setRenderTarget(RenderTarget target)
{
    Q_D(QSGPaintedItem);

    if (d->renderTarget == target)
        return;

    d->renderTarget = target;
    update();

    emit renderTargetChanged();
}

/*!
    \fn virtual void QSGPaintedItem::paint(QPainter *painter) = 0

    This function, which is usually called by the QML Scene Graph, paints the
    contents of an item in local coordinates.

    The function is called after the item has been filled with the fillColor.

    Reimplement this function in a QSGPaintedItem subclass to provide the
    item's painting implementation, using \a painter.
*/

/*!
    This function is called after the item's geometry has changed.
*/
void QSGPaintedItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    Q_D(QSGPaintedItem);
    d->geometryDirty = true;
    QSGItem::geometryChanged(newGeometry, oldGeometry);
}


/*!
    This function is called when the Scene Graph node associated to the item needs to
    be updated.
*/
QSGNode *QSGPaintedItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);
    Q_D(QSGPaintedItem);

    if (width() <= 0 || height() <= 0) {
        delete oldNode;
        return 0;
    }

    QSGPainterNode *node = static_cast<QSGPainterNode *>(oldNode);
    if (!node)
        node = new QSGPainterNode(this);

    node->setPreferredRenderTarget(d->renderTarget);
    node->setSize(QSize(d->width, d->height));
    node->setSmoothPainting(d->smooth);
    node->setLinearFiltering(d->smooth);
    node->setOpaquePainting(d->opaquePainting);
    node->setFillColor(d->fillColor);
    node->setDirty(d->contentsDirty || d->geometryDirty, d->dirtyRect);
    node->update();

    d->contentsDirty = false;
    d->geometryDirty = false;
    d->dirtyRect = QRect();

    return node;
}

QT_END_NAMESPACE
