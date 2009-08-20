/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

/*!
    \class QGraphicsEffect
    \brief The QGraphicsEffect class is the base class for all graphics effects.
    \since 4.6
    \ingroup multimedia
    \ingroup graphicsview-api

    Effects alter the appearance of elements by hooking into the rendering
    pipeline and operating between the source (e.g., a QGraphicsPixmapItem),
    and the destination device (e.g., QGraphicsView's viewport). Effects can be
    disabled by calling setEnabled(); if the effect is disabled, the source is
    rendered directly.

    If you want to add a visual effect to a QGraphicsItem, you can either use
    one of the standard effects, or create your own effect by making a subclass
    of QGraphicsEffect.

    Qt provides several standard effects, including:

    \list
    \o QGraphicsGrayScaleEffect - renders the item in shades of gray
    \o QGraphicsColorizeEffect - renders the item in shades of any given color
    \o QGraphicsPixelizeEffect - pixelizes the item with any pixel size
    \o QGraphicsBlurEffect - blurs the item by a given radius
    \o QGraphicsDropShadowEffect - renders a dropshadow behind the item
    \endlist

    If all you want is to add an effect to an item, you should visit the
    documentation for the specific effect to learn more about how each effect
    can be used.

    If you want to create your own custom effect, you can start by creating a
    subclass of QGraphicsEffect (or any of the existing effects), and
    reimplement the virtual function draw(). This function is called whenever
    the effect needs to redraw. draw() has two arguments: the painter, and a
    pointer to the source (QGraphicsEffectSource). The source provides extra
    context information, such as a pointer to the item that is rendering the
    effect, any cached pixmap data, and the device rect bounds. See the draw()
    documentation for more details. You can also get a pointer to the current
    source by calling source().

    If your effect changes, you can call update() to request a redraw. If your
    custom effect changes the bounding rectangle of the source (e.g., a radial
    glow effect may need to apply an extra margin), you can reimplement the
    virtual boundingRectFor() function, and call updateBoundingRect() to notify
    the framework whenever this rectangle changes. The virtual
    sourceBoundingRectChanged() function is called to notify the effects that
    the source's bounding rectangle has changed (e.g., if the source is a
    QGraphicsRectItem, then if the rectangle parameters have changed).

    \sa QGraphicsItem::setGraphicsEffect()
*/

#include "qgraphicseffect_p.h"

#include <QtGui/qimage.h>
#include <QtGui/qpainter.h>
#include <QtCore/qrect.h>

QT_BEGIN_NAMESPACE

QGraphicsEffectSource::QGraphicsEffectSource(QGraphicsEffectSourcePrivate &dd, QObject *parent)
    : QObject(dd, parent)
{}

QGraphicsEffectSource::~QGraphicsEffectSource()
{}

QRect QGraphicsEffectSource::deviceRect() const
{
    return d_func()->deviceRect();
}

QRectF QGraphicsEffectSource::boundingRect(Qt::CoordinateSystem system) const
{
    return d_func()->boundingRect(system);
}

const QGraphicsItem *QGraphicsEffectSource::graphicsItem() const
{
    return d_func()->graphicsItem();
}

const QStyleOption *QGraphicsEffectSource::styleOption() const
{
    return d_func()->styleOption();
}

void QGraphicsEffectSource::draw(QPainter *painter)
{
    d_func()->draw(painter);
}

void QGraphicsEffectSource::update()
{
    d_func()->update();
}

bool QGraphicsEffectSource::isPixmap() const
{
    return d_func()->isPixmap();
}

QPixmap QGraphicsEffectSource::pixmap(Qt::CoordinateSystem system, QPoint *offset) const
{
    return d_func()->pixmap(system, offset);
}

/*!
    Constructs a new QGraphicsEffect instance.
*/
QGraphicsEffect::QGraphicsEffect(QObject *parent)
    : QObject(*new QGraphicsEffectPrivate, parent)
{
}

/*!
    \internal
*/
QGraphicsEffect::QGraphicsEffect(QGraphicsEffectPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
}

/*!
    Removes the effect from the source, and destroys the graphics effect.
*/
QGraphicsEffect::~QGraphicsEffect()
{
    Q_D(QGraphicsEffect);
    d->setGraphicsEffectSource(0);
}

/*!
    Returns the bounding rectangle for this effect (i.e., the bounding
    rectangle of the source, adjusted by any margins applied by the effect
    itself).

    \sa boundingRectFor(), updateBoundingRect()
*/
QRectF QGraphicsEffect::boundingRect() const
{
    Q_D(const QGraphicsEffect);
    if (d->source)
        return boundingRectFor(d->source->boundingRect());
    return QRectF();
}

/*!
    Returns the bounding rectangle for this effect, given the provided source
    \a rect. When writing you own custom effect, you must call
    updateBoundingRect() whenever any parameters are changed that may cause
    this this function to return a different value.

    \sa boundingRect()
*/
QRectF QGraphicsEffect::boundingRectFor(const QRectF &rect) const
{
    return rect;
}

/*!
    \property QGraphicsEffect::enabled
    \brief whether the effect is enabled or not.

    If an effect is disabled, the source will be rendered with as normal, with
    no interference from the effect. If the effect is enabled (default), the
    source will be rendered with the effect applied.

    This property is provided so that you can disable certain effects on slow
    platforms, in order to ensure that the user interface is responsive.
*/
bool QGraphicsEffect::isEnabled() const
{
    Q_D(const QGraphicsEffect);
    return d->isEnabled;
}

void QGraphicsEffect::setEnabled(bool enable)
{
    Q_D(QGraphicsEffect);
    if (d->isEnabled == enable)
        return;

    d->isEnabled = enable;
    if (d->source)
        d->source->update();
    emit enabledChanged(enable);
}

/*!
    Returns a pointer to the source, which provides extra context information
    that can be useful for the effect.

    \sa draw()
*/
QGraphicsEffectSource *QGraphicsEffect::source() const
{
    Q_D(const QGraphicsEffect);
    return d->source;
}

/*!
    This function notifies the effect framework that the effect's bounding
    rectangle has changed. As a custom effect author, you must call this
    function whenever you change any parameters that will cause the virtual
    boundingRectFor() function to return a different value.

    \sa boundingRectFor(), boundingRect()
*/
void QGraphicsEffect::updateBoundingRect()
{
    Q_D(QGraphicsEffect);
    if (d->source)
        d->source->update();
}

/*!
    This virtual function is called by QGraphicsEffect to notify the effect
    that the source has changed. If the effect applies any cache, then this
    cache must be purged in order to reflect the new appearance of the source.

    The \a flags describes what has changed.
*/
void QGraphicsEffect::sourceChanged(ChangeFlags flags)
{
    Q_UNUSED(flags);
}

QGraphicsGrayscaleEffect::QGraphicsGrayscaleEffect(QObject *parent)
    : QGraphicsEffect(*new QGraphicsGrayscaleEffectPrivate, parent)
{
}

QGraphicsGrayscaleEffect::~QGraphicsGrayscaleEffect()
{
}

void QGraphicsGrayscaleEffect::draw(QPainter *painter, QGraphicsEffectSource *source)
{
    Q_D(QGraphicsGrayscaleEffect);
    QPoint offset;
    if (source->isPixmap()) {
        // No point in drawing in device coordinates (pixmap will be scaled anyways).
        const QPixmap pixmap = source->pixmap(Qt::LogicalCoordinates, &offset);
        d->filter->draw(painter, offset, pixmap);
        return;
    }

    // Draw pixmap in device coordinates to avoid pixmap scaling;
    const QPixmap pixmap = source->pixmap(Qt::DeviceCoordinates, &offset);
    QTransform restoreTransform = painter->worldTransform();
    painter->setWorldTransform(QTransform());
    d->filter->draw(painter, offset, pixmap);
    painter->setWorldTransform(restoreTransform);

}

QGraphicsColorizeEffect::QGraphicsColorizeEffect(QObject *parent)
    : QGraphicsEffect(*new QGraphicsColorizeEffectPrivate, parent)
{
}

QGraphicsColorizeEffect::~QGraphicsColorizeEffect()
{
}

QColor QGraphicsColorizeEffect::color() const
{
    Q_D(const QGraphicsColorizeEffect);
    return d->filter->color();
}

void QGraphicsColorizeEffect::setColor(const QColor &c)
{
    Q_D(QGraphicsColorizeEffect);
    if (d->filter->color() == c)
        return;

    d->filter->setColor(c);
    emit colorChanged(c);
}

void QGraphicsColorizeEffect::draw(QPainter *painter, QGraphicsEffectSource *source)
{
    Q_D(QGraphicsColorizeEffect);
    QPoint offset;
    if (source->isPixmap()) {
        // No point in drawing in device coordinates (pixmap will be scaled anyways).
        const QPixmap pixmap = source->pixmap(Qt::LogicalCoordinates, &offset);
        d->filter->draw(painter, offset, pixmap);
        return;
    }

    // Draw pixmap in deviceCoordinates to avoid pixmap scaling.
    const QPixmap pixmap = source->pixmap(Qt::DeviceCoordinates, &offset);
    QTransform restoreTransform = painter->worldTransform();
    painter->setWorldTransform(QTransform());
    d->filter->draw(painter, offset, pixmap);
    painter->setWorldTransform(restoreTransform);
}

QGraphicsPixelizeEffect::QGraphicsPixelizeEffect(QObject *parent)
    : QGraphicsEffect(*new QGraphicsPixelizeEffectPrivate, parent)
{
}

QGraphicsPixelizeEffect::~QGraphicsPixelizeEffect()
{
}

int QGraphicsPixelizeEffect::pixelSize() const
{
    Q_D(const QGraphicsPixelizeEffect);
    return d->pixelSize;
}

void QGraphicsPixelizeEffect::setPixelSize(int size)
{
    Q_D(QGraphicsPixelizeEffect);
    if (d->pixelSize == size)
        return;

    d->pixelSize = size;
    emit pixelSizeChanged(size);
}

static inline void pixelize(QImage *image, int pixelSize)
{
    Q_ASSERT(pixelSize > 0);
    Q_ASSERT(image);
    int width = image->width();
    int height = image->height();
    for (int y = 0; y < height; y += pixelSize) {
        int ys = qMin(height - 1, y + pixelSize / 2);
        QRgb *sbuf = reinterpret_cast<QRgb*>(image->scanLine(ys));
        for (int x = 0; x < width; x += pixelSize) {
            int xs = qMin(width - 1, x + pixelSize / 2);
            QRgb color = sbuf[xs];
            for (int yi = 0; yi < qMin(pixelSize, height - y); ++yi) {
                QRgb *buf = reinterpret_cast<QRgb*>(image->scanLine(y + yi));
                for (int xi = 0; xi < qMin(pixelSize, width - x); ++xi)
                    buf[x + xi] = color;
            }
        }
    }
}

void QGraphicsPixelizeEffect::draw(QPainter *painter, QGraphicsEffectSource *source)
{
    Q_D(QGraphicsPixelizeEffect);
    if (d->pixelSize <= 0) {
        source->draw(painter);
        return;
    }

    QPoint offset;
    if (source->isPixmap()) {
        const QPixmap pixmap = source->pixmap(Qt::LogicalCoordinates, &offset);
        QImage image = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);
        pixelize(&image, d->pixelSize);
        painter->drawImage(offset, image);
        return;
    }

    // Draw pixmap in device coordinates to avoid pixmap scaling.
    const QPixmap pixmap = source->pixmap(Qt::DeviceCoordinates, &offset);

    // pixelize routine
    QImage image = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);
    pixelize(&image, d->pixelSize);

    QTransform restoreTransform = painter->worldTransform();
    painter->setWorldTransform(QTransform());
    painter->drawImage(offset, image);
    painter->setWorldTransform(restoreTransform);
}

QGraphicsBlurEffect::QGraphicsBlurEffect(QObject *parent)
    : QGraphicsEffect(*new QGraphicsBlurEffectPrivate, parent)
{
}

QGraphicsBlurEffect::~QGraphicsBlurEffect()
{
}

// Blur the image according to the blur radius
// Based on exponential blur algorithm by Jani Huhtanen
// (maximum radius is set to 16)
static QImage blurred(const QImage& image, const QRect& rect, int radius)
{
    int tab[] = { 14, 10, 8, 6, 5, 5, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 };
    int alpha = (radius < 1)  ? 16 : (radius > 17) ? 1 : tab[radius-1];

    QImage result = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    int r1 = rect.top();
    int r2 = rect.bottom();
    int c1 = rect.left();
    int c2 = rect.right();

    int bpl = result.bytesPerLine();
    int rgba[4];
    unsigned char* p;

    for (int col = c1; col <= c2; col++) {
        p = result.scanLine(r1) + col * 4;
        for (int i = 0; i < 4; i++)
            rgba[i] = p[i] << 4;

        p += bpl;
        for (int j = r1; j < r2; j++, p += bpl)
            for (int i = 0; i < 4; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    for (int row = r1; row <= r2; row++) {
        p = result.scanLine(row) + c1 * 4;
        for (int i = 0; i < 4; i++)
            rgba[i] = p[i] << 4;

        p += 4;
        for (int j = c1; j < c2; j++, p += 4)
            for (int i = 0; i < 4; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    for (int col = c1; col <= c2; col++) {
        p = result.scanLine(r2) + col * 4;
        for (int i = 0; i < 4; i++)
            rgba[i] = p[i] << 4;

        p -= bpl;
        for (int j = r1; j < r2; j++, p -= bpl)
            for (int i = 0; i < 4; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    for (int row = r1; row <= r2; row++) {
        p = result.scanLine(row) + c2 * 4;
        for (int i = 0; i < 4; i++)
            rgba[i] = p[i] << 4;

        p -= 4;
        for (int j = c1; j < c2; j++, p -= 4)
            for (int i = 0; i < 4; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    return result;
}

int QGraphicsBlurEffect::blurRadius() const
{
    Q_D(const QGraphicsBlurEffect);
    return d->filter->radius();
}

void QGraphicsBlurEffect::setBlurRadius(int radius)
{
    Q_D(QGraphicsBlurEffect);
    if (d->filter->radius() == radius)
        return;

    d->filter->setRadius(radius);
    updateBoundingRect();
    emit blurRadiusChanged(radius);
}

QRectF QGraphicsBlurEffect::boundingRectFor(const QRectF &rect) const
{
    Q_D(const QGraphicsBlurEffect);
    return d->filter->boundingRectFor(rect);
}

void QGraphicsBlurEffect::draw(QPainter *painter, QGraphicsEffectSource *source)
{
    Q_D(QGraphicsBlurEffect);
    if (d->filter->radius() <= 0) {
        source->draw(painter);
        return;
    }

    QPoint offset;
    if (source->isPixmap()) {
        // No point in drawing in device coordinates (pixmap will be scaled anyways).
        const QPixmap pixmap = source->pixmap(Qt::LogicalCoordinates, &offset);
        d->filter->draw(painter, offset, pixmap);
        return;
    }

    // Draw pixmap in device coordinates to avoid pixmap scaling.
    const QPixmap pixmap = source->pixmap(Qt::DeviceCoordinates, &offset);
    QTransform restoreTransform = painter->worldTransform();
    painter->setWorldTransform(QTransform());
    d->filter->draw(painter, offset, pixmap);
    painter->setWorldTransform(restoreTransform);
}

QGraphicsDropShadowEffect::QGraphicsDropShadowEffect(QObject *parent)
    : QGraphicsEffect(*new QGraphicsDropShadowEffectPrivate, parent)
{
}

QGraphicsDropShadowEffect::~QGraphicsDropShadowEffect()
{
}

QPointF QGraphicsDropShadowEffect::shadowOffset() const
{
    Q_D(const QGraphicsDropShadowEffect);
    return d->offset;
}

void QGraphicsDropShadowEffect::setShadowOffset(const QPointF &ofs)
{
    Q_D(QGraphicsDropShadowEffect);
    if (d->offset == ofs)
        return;

    d->offset = ofs;
    updateBoundingRect();
    emit shadowOffsetChanged(ofs);
}

int QGraphicsDropShadowEffect::blurRadius() const
{
    Q_D(const QGraphicsDropShadowEffect);
    return d->radius;
}

void QGraphicsDropShadowEffect::setBlurRadius(int blurRadius)
{
    Q_D(QGraphicsDropShadowEffect);
    if (d->radius == blurRadius)
        return;

    d->radius = blurRadius;
    updateBoundingRect();
    emit blurRadiusChanged(blurRadius);
}

qreal QGraphicsDropShadowEffect::opacity() const
{
    Q_D(const QGraphicsDropShadowEffect);
    return d->alpha;
}

void QGraphicsDropShadowEffect::setOpacity(qreal opacity)
{
    Q_D(QGraphicsDropShadowEffect);
    if (qFuzzyCompare(d->alpha, opacity))
        return;

    d->alpha = opacity;
    emit opacityChanged(opacity);
}

QRectF QGraphicsDropShadowEffect::boundingRectFor(const QRectF &rect) const
{
    Q_D(const QGraphicsDropShadowEffect);
    QRectF shadowRect = rect.translated(d->offset);
    QRectF blurRect = shadowRect;
    qreal delta = d->radius * 3;
    blurRect.adjust(-delta, -delta, delta, delta);
    blurRect |= rect;
    return blurRect;
}

void QGraphicsDropShadowEffect::draw(QPainter *painter, QGraphicsEffectSource *source)
{
    Q_D(QGraphicsDropShadowEffect);
    if (d->radius <= 0 && d->offset.isNull()) {
        source->draw(painter);
        return;
    }

    const QTransform &transform = painter->worldTransform();
    const QPointF offset(d->offset.x() * transform.m11(), d->offset.y() * transform.m22());
    const QPoint shadowOffset = offset.toPoint();
    const QRectF sourceRect = source->boundingRect(Qt::DeviceCoordinates);
    const QRectF shadowRect = sourceRect.translated(offset);

    QRectF blurRect = shadowRect;
    qreal delta = d->radius * 3;
    blurRect.adjust(-delta, -delta, delta, delta);
    blurRect |= sourceRect;

    QRect effectRect = blurRect.toAlignedRect();
    const QRect deviceRect = source->deviceRect();
    const bool fullyInsideDeviceRect = effectRect.x() >= deviceRect.x()
                                    && effectRect.right() <= deviceRect.right()
                                    && effectRect.y() >= deviceRect.y()
                                    && effectRect.bottom() <= deviceRect.bottom();
    if (!fullyInsideDeviceRect) {
        // Clip to device rect to avoid huge pixmaps.
        effectRect &= source->deviceRect();
        effectRect |= effectRect.translated(-shadowOffset);
        if (effectRect.isEmpty())
            return; // nothing to paint;
    }

    QPixmap pixmap(effectRect.size());
    pixmap.fill(Qt::transparent);
    QPainter pixmapPainter(&pixmap);
    pixmapPainter.setRenderHints(painter->renderHints());
    pixmapPainter.setWorldTransform(painter->worldTransform());
    if (effectRect.x() != 0 || effectRect.y() != 0)
        pixmapPainter.translate(-effectRect.topLeft());
    source->draw(&pixmapPainter);
    pixmapPainter.end();

    QImage img = pixmap.toImage();
    QImage shadowImage(img.size(), QImage::Format_ARGB32);
    shadowImage.fill(qRgba(0, 0, 0, d->alpha * 255));
    shadowImage.setAlphaChannel(img.alphaChannel());
    shadowImage = blurred(shadowImage, shadowImage.rect(), d->radius);

    // Draw using an untransformed painter.
    QTransform restoreTransform = painter->worldTransform();
    painter->setWorldTransform(QTransform());
    painter->drawImage(effectRect.topLeft() + shadowOffset, shadowImage);
    painter->drawPixmap(effectRect.topLeft(), pixmap);
    painter->setWorldTransform(restoreTransform);
}

QT_END_NAMESPACE

