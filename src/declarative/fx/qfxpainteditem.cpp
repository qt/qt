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

#include "qfxpainteditem.h"
#include "qfxpainteditem_p.h"

#include <QDebug>
#include <QPen>
#include <QFile>
#include <QEvent>
#include <QApplication>
#include <QGraphicsSceneMouseEvent>

QT_BEGIN_NAMESPACE

/*!
    \class QFxPaintedItem
    \brief The QFxPaintedItem class is an abstract base class for QFxView items that want cached painting.
    \ingroup group_coreitems

    This is a convenience class for implementing items that paint their contents
    using a QPainter.  The contents of the item are are cached behind the scenes.
    The dirtyCache() function should be called if the contents change to
    ensure the cache is refreshed the next time painting occurs.

    To subclass QFxPaintedItem, you must reimplement drawContents() to draw
    the contents of the item.
*/

/*!
    \fn void QFxPaintedItem::drawContents(QPainter *painter, const QRect &rect)

    This function is called when the cache needs to be refreshed. When
    sub-classing QFxPaintedItem this function should be implemented so as to
    paint the contents of the item using the given \a painter for the
    area of the contents specified by \a rect.
*/

/*!
    \property QFxPaintedItem::contentsSize
    \brief The size of the contents

    The contents size is the size of the item in regards to how it is painted
    using the drawContents() function.  This is distinct from the size of the
    item in regards to height() and width().
*/

/*!
    \property QFxPaintedItem::smooth
    \brief Setting for whether smooth scaling is enabled.
*/

/*!
    Marks areas of the cache that intersect with the given \a rect as dirty and
    in need of being refreshed.

    \sa clearCache()
*/
void QFxPaintedItem::dirtyCache(const QRect& rect)
{
    Q_D(QFxPaintedItem);
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
void QFxPaintedItem::clearCache()
{
    Q_D(QFxPaintedItem);
    qDeleteAll(d->imagecache);
    d->imagecache.clear();
}

/*!
    Returns if smooth scaling of the cache contents is enabled.

    \sa setSmooth()
*/
bool QFxPaintedItem::isSmooth() const
{
    Q_D(const QFxPaintedItem);
    return d->smooth;
}

/*!
    Returns the size of the contents.

    \sa setContentsSize()
*/
QSize QFxPaintedItem::contentsSize() const
{
    Q_D(const QFxPaintedItem);
    return d->contentsSize;
}

/*!
    If \a smooth is true sets the image item to enable smooth scaling of
    the cache contents.

    \sa isSmooth()
*/
void QFxPaintedItem::setSmooth(bool smooth)
{
    Q_D(QFxPaintedItem);
    if (d->smooth == smooth) return;
    d->smooth = smooth;
    update();
}

/*!
    Sets the size of the contents to the given \a size.

    \sa contentsSize()
*/
void QFxPaintedItem::setContentsSize(const QSize &size)
{
    Q_D(QFxPaintedItem);
    if (d->contentsSize == size) return;
    d->contentsSize = size;
    clearCache();
    update();
}

/*!
    Constructs a new QFxPaintedItem with the given \a parent.
*/
QFxPaintedItem::QFxPaintedItem(QFxItem *parent)
  : QFxItem(*(new QFxPaintedItemPrivate), parent)
{
    init();
}

/*!
    \internal
    Constructs a new QFxPaintedItem with the given \a parent and
    initialized private data member \a dd.
*/
QFxPaintedItem::QFxPaintedItem(QFxPaintedItemPrivate &dd, QFxItem *parent)
  : QFxItem(dd, parent)
{
    init();
}

/*!
    Destroys the image item.
*/
QFxPaintedItem::~QFxPaintedItem()
{
}

/*!
    \internal
*/
void QFxPaintedItem::init()
{
    connect(this,SIGNAL(widthChanged()),this,SLOT(clearCache()));
    connect(this,SIGNAL(heightChanged()),this,SLOT(clearCache()));
    connect(this,SIGNAL(visibleChanged()),this,SLOT(clearCache()));
}

/*!
    \reimp
*/
void QFxPaintedItem::paintContents(QPainter &p)
{
    Q_D(QFxPaintedItem);
    const QRect content(QPoint(0,0),d->contentsSize);
    if (content.width() <= 0 || content.height() <= 0)
        return;

    bool oldAntiAliasing = p.testRenderHint(QPainter::Antialiasing);
    bool oldSmoothPixmap = p.testRenderHint(QPainter::SmoothPixmapTransform);
    if (oldAntiAliasing)
        p.setRenderHints(QPainter::Antialiasing, false); // cannot stitch properly otherwise
    if (d->smooth)
        p.setRenderHints(QPainter::SmoothPixmapTransform, true);
    QRectF clipf = p.clipRegion().boundingRect();
    if (clipf.isEmpty())
        clipf = mapToScene(content); // ### Inefficient: Maps toScene and then fromScene
    else
        clipf = mapToScene(clipf);

    const QRect clip = mapFromScene(clipf).toRect();

    QRegion topaint(clip);
    topaint &= content;
    QRegion uncached(content);

    int cachesize=0;
    for (int i=0; i<d->imagecache.count(); ++i) {
        QRect area = d->imagecache[i]->area;
        if (topaint.contains(area)) {
            QRectF target(area.x(), area.y(), area.width(), area.height());
            p.drawPixmap(target.toRect(), d->imagecache[i]->image);
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
        for (int i = 0; i < rects.count(); ++i) {
            const QRect &r = rects.at(i);
            QPixmap img(r.size());
            if (d->fillColor.isValid())
                img.fill(d->fillColor);
            {
                QPainter qp(&img);
                qp.translate(-r.x(),-r.y());
                drawContents(&qp, r);
            }
            QFxPaintedItemPrivate::ImageCacheItem *newitem = new QFxPaintedItemPrivate::ImageCacheItem;
            newitem->area = r;
            newitem->image = img;
            d->imagecache.append(newitem);
            QRectF target(r.x(), r.y(), r.width(), r.height());
            p.drawPixmap(target.toRect(), newitem->image);
        }
    }

    if (oldAntiAliasing)
        p.setRenderHints(QPainter::Antialiasing, oldAntiAliasing);
    if (d->smooth)
        p.setRenderHints(QPainter::SmoothPixmapTransform, oldSmoothPixmap);
}

/*!
  \qmlproperty int PaintedItem::cacheSize

  This property holds the maximum number of pixels of image cache to
  allow. The default is 0.1 megapixels. The cache will not be larger
  than the (unscaled) size of the item.
*/

/*!
  \property QFxPaintedItem::cacheSize

  The maximum number of pixels of image cache to allow. The default
  is 0.1 megapixels. The cache will not be larger than the (unscaled)
  size of the QFxPaintedItem.
*/
int QFxPaintedItem::cacheSize() const
{
    Q_D(const QFxPaintedItem);
    return d->max_imagecache_size;
}

void QFxPaintedItem::setCacheSize(int pixels)
{
    Q_D(QFxPaintedItem);
    if (pixels < d->max_imagecache_size) {
        int cachesize=0;
        for (int i=0; i<d->imagecache.count(); ++i) {
            QRect area = d->imagecache[i]->area;
            cachesize += area.width()*area.height();
        }
        while (d->imagecache.count() && cachesize > pixels) {
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
            d->imagecache.removeAt(oldest);
        }
    }
    d->max_imagecache_size = pixels;
}

/*!
    \property QFxPaintedItem::fillColor

    The color to be used to fill the item prior to calling drawContents().
    By default, this is Qt::transparent.

    Performance improvements can be achieved if subclasses call this with either an
    invalid color (QColor()), or an appropriate solid color.
*/
void QFxPaintedItem::setFillColor(const QColor& c)
{
    Q_D(QFxPaintedItem);
    if (d->fillColor == c)
        return;
    d->fillColor = c;
    emit fillColorChanged();
    update();
}

QColor QFxPaintedItem::fillColor() const
{
    Q_D(const QFxPaintedItem);
    return d->fillColor;
}


QT_END_NAMESPACE
