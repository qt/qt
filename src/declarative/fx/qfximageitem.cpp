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

#include "qfximageitem.h"
#include "qfximageitem_p.h"

#include <QDebug>
#include <QPen>
#include <QFile>
#include <QEvent>
#include <QApplication>
#include <QGraphicsSceneMouseEvent>

#if defined(QFX_RENDER_OPENGL2)
#include <QtOpenGL/qglframebufferobject.h>
#include <glsave.h>
#endif

QT_BEGIN_NAMESPACE

/*!
    \class QFxImageItem
    \brief The QFxImageItem class is an abstract base class for QFxView items that render using QPainter.
    \ingroup group_coreitems

    This is a convenience class allowing easy use of a QPainter within a custom
    item.  The contents of the item are are cached behind the scenes.
    The dirtyCache() function should be called if the contents change to
    ensure the cache is refreshed the next time painting occurs.

    To subclass QFxImageItem, you must reimplement drawContents() to draw
    the contents of the item.
*/

/*!
    \fn void QFxImageItem::drawContents(QPainter *painter, const QRect &rect)

    This function is called when the cache needs to be refreshed. When
    sub-classing QFxImageItem this function should be implemented so as to
    paint the contents of the item using the given \a painter for the
    area of the contents specified by \a rect.
*/

/*!
    \property QFxImageItem::contentsSize
    \brief The size of the contents

    The contents size is the size of the item in regards to how it is painted
    using the drawContents() function.  This is distinct from the size of the
    item in regards to height() and width().
*/

/*!
    \property QFxImageItem::smooth
    \brief Setting for whether smooth scaling is enabled.
*/

/*!
    Marks areas of the cache that intersect with the given \a rect as dirty and
    in need of being refreshed.

    \sa clearCache()
*/
void QFxImageItem::dirtyCache(const QRect& rect)
{
    Q_D(QFxImageItem);
    for (int i=0; i < d->imagecache.count(); ) {
        if (d->imagecache[i]->area.intersects(rect)) {
            d->imagecache.removeAt(i);
        } else {
            ++i;
        }
    }
}

/*!
    Marks the entirety of the contents cache as dirty.

    \sa dirtyCache()
*/
void QFxImageItem::clearCache()
{
    Q_D(QFxImageItem);
    foreach (QFxImageItemPrivate::ImageCacheItem* i, d->imagecache)
        delete i;
    d->imagecache.clear();
}

/*!
    Returns if smooth scaling of the cache contents is enabled.

    \sa setSmooth()
*/
bool QFxImageItem::isSmooth() const
{
    Q_D(const QFxImageItem);
    return d->smooth;
}

/*!
    Returns the size of the contents.

    \sa setContentsSize()
*/
QSize QFxImageItem::contentsSize() const
{
    Q_D(const QFxImageItem);
    return d->contentsSize;
}

/*!
    If \a smooth is true sets the image item to enable smooth scaling of
    the cache contents.

    \sa isSmooth()
*/
void QFxImageItem::setSmooth(bool smooth)
{
    Q_D(QFxImageItem);
    if (d->smooth == smooth) return;
    d->smooth = smooth;
    clearCache();
    update();
}

/*!
    Sets the size of the contents to the given \a size.

    \sa contentsSize()
*/
void QFxImageItem::setContentsSize(const QSize &size)
{
    Q_D(QFxImageItem);
    if (d->contentsSize == size) return;
    d->contentsSize = size;
    clearCache();
    update();
}

/*!
    Constructs a new QFxImageItem with the given \a parent.
*/
QFxImageItem::QFxImageItem(QFxItem *parent)
  : QFxItem(*(new QFxImageItemPrivate), parent)
{
    init();
}

/*!
    \internal
    Constructs a new QFxImageItem with the given \a parent and
    initialized private data member \a dd.
*/
QFxImageItem::QFxImageItem(QFxImageItemPrivate &dd, QFxItem *parent)
  : QFxItem(dd, parent)
{
    init();
}

/*!
    Destroys the image item.
*/
QFxImageItem::~QFxImageItem()
{
}

/*!
    \internal
*/
void QFxImageItem::init()
{
    connect(this,SIGNAL(widthChanged()),this,SLOT(clearCache()));
    connect(this,SIGNAL(heightChanged()),this,SLOT(clearCache()));
    connect(this,SIGNAL(visibleChanged()),this,SLOT(clearCache()));
}

#if defined(QFX_RENDER_QPAINTER)
/*!
    \reimp
*/
void QFxImageItem::paintContents(QPainter &p)
#elif defined(QFX_RENDER_OPENGL)
/*!
    \reimp
*/
void QFxImageItem::paintGLContents(GLPainter &p)
#else
#error "What render?"
#endif
{
    Q_D(QFxImageItem);
    const QRect content(QPoint(0,0),d->contentsSize);
    if (content.width() <= 0 || content.height() <= 0)
        return;

#if defined(QFX_RENDER_QPAINTER)
    if (d->smooth) {
        p.save();
        p.setRenderHints(QPainter::Antialiasing, true);
        p.setRenderHints(QPainter::SmoothPixmapTransform, true);
    }
    QRectF clipf = p.clipRegion().boundingRect();
    if (clipf.isEmpty())
        clipf = mapToScene(content); // ### Inefficient: Maps toScene and then fromScene
    else
        clipf = mapToScene(clipf);

#elif defined(QFX_RENDER_OPENGL2)
    p.useTextureShader();
    const QRectF clipf = p.sceneClipRect;

#elif defined(QFX_RENDER_OPENGL1)
    p.useTextureShader();
    const QRectF clipf = p.sceneClipRect;
#endif

    qreal hscale = widthValid() ? qreal(width()) / content.width() : heightValid() ? qreal(height()) / content.height() : 1.0;
    qreal vscale = heightValid() ? qreal(height()) / content.height() : widthValid() ? qreal(width()) / content.width() : 1.0;
    const QRect clip = mapFromScene(QRectF(clipf.x()/hscale,clipf.y()/vscale,clipf.width()/hscale,clipf.height()/vscale)).toRect();

    QRegion topaint(clip);
    topaint &= content;
    QRegion uncached(content);

#if defined(QFX_RENDER_OPENGL2)
    glEnableVertexAttribArray(SingleTextureShader::Vertices);
    glEnableVertexAttribArray(SingleTextureShader::TextureCoords);
#elif defined(QFX_RENDER_OPENGL1)
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#endif

    int cachesize=0;
    for (int i=0; i<d->imagecache.count(); ++i) {
        QRect area = d->imagecache[i]->area;
        if (topaint.contains(area)) {
            QRectF target(area.x()*hscale, area.y()*vscale, area.width()*hscale, area.height()*vscale);
            p.drawImage(target.toRect(), d->imagecache[i]->image);
            topaint -= area;
            d->imagecache[i]->age=0;
        } else {
            d->imagecache[i]->age++;
        }
        cachesize += area.width()*area.height();
        uncached -= area;
    }

    if (!topaint.isEmpty()) {
        // Find a sensible larger area, otherwise will paint lots of tiny images.
        QRect biggerrect = topaint.boundingRect().adjusted(-64,-64,128,128);
        cachesize += biggerrect.width() * biggerrect.height();
        while (d->imagecache.count() && cachesize > d->max_imagecache_size) {
            int oldest=-1;
            int age=-1;
            for (int i=0; i<d->imagecache.count(); ++i) {
                int a = d->imagecache[i]->age;
                if (a > age) {
                    oldest = i;
                    age = a;
                }
            }
            cachesize -= d->imagecache[oldest]->area.width()*d->imagecache[oldest]->area.height();
            uncached += d->imagecache[oldest]->area;
            d->imagecache.removeAt(oldest);
        }
        const QRegion bigger = QRegion(biggerrect) & uncached;
        const QVector<QRect> rects = bigger.rects();
        foreach (QRect r, rects) {
#if defined(QFX_RENDER_QPAINTER)
            QImage img(r.size(),QImage::Format_ARGB32_Premultiplied);
#else
            QImage img(r.size(),QImage::Format_ARGB32);
#endif
            img.fill(0);
            {
                QPainter qp(&img);
                qp.translate(-r.x(),-r.y());
                drawContents(&qp, r);
            }
            QFxImageItemPrivate::ImageCacheItem *newitem = new QFxImageItemPrivate::ImageCacheItem;
            newitem->area = r;
#if defined(QFX_RENDER_QPAINTER)
            newitem->image = QSimpleCanvasConfig::Image(QSimpleCanvasConfig::toImage(img));
#else
            newitem->image.setImage(img);
#endif
            d->imagecache.append(newitem);
            QRectF target(r.x()*hscale, r.y()*vscale, r.width()*hscale, r.height()*vscale);
            p.drawImage(target.toRect(), newitem->image);
        }
    }
#if defined(QFX_RENDER_OPENGL2)
    glDisableVertexAttribArray(SingleTextureShader::Vertices);
    glDisableVertexAttribArray(SingleTextureShader::TextureCoords);
#elif defined(QFX_RENDER_OPENGL1)
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
#if defined(QFX_RENDER_QPAINTER)
    if (d->smooth) 
        p.restore();
#endif
}

QT_END_NAMESPACE
