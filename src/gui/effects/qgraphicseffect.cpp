/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

/*!
    \class QGraphicsEffect
    \brief The QGraphicsEffect class is the base class for all graphics
           effects.
    \since 4.6
    \ingroup multimedia
    \ingroup graphicsview-api

    Effects alter the appearance of elements by hooking into the rendering
    pipeline and operating between the source (e.g., a QGraphicsPixmapItem)
    and the destination device (e.g., QGraphicsView's viewport). Effects can be
    disabled by calling setEnabled(false). If effects are disabled, the source
    is rendered directly.

    To add a visual effect to a QGraphicsItem, for example, you can use one of
    the standard effects, or alternately, create your own effect by creating a
    subclass of QGraphicsEffect. The effect can then be installed on the item
    using QGraphicsItem::setGraphicsEffect().

    Qt provides the following standard effects:

    \list
    \o QGraphicsBlurEffect - blurs the item by a given radius
    \o QGraphicsDropShadowEffect - renders a dropshadow behind the item
    \o QGraphicsColorizeEffect - renders the item in shades of any given color
    \o QGraphicsOpacityEffect - renders the item with an opacity
    \o QGraphicsPixelizeEffect - pixelizes the item with any pixel size
    \o QGraphicsGrayscaleEffect - renders the item in shades of gray
    \o QGraphicsBloomEffect - applies a blooming / glowing effect
    \endlist

    \img graphicseffect-effects.png
    \img graphicseffect-widget.png

    For more information on how to use each effect, refer to the specific
    effect's documentation.

    To create your own custom effect, create a subclass of QGraphicsEffect (or
    any other existing effects) and reimplement the virtual function draw().
    This function is called whenever the effect needs to redraw. The draw()
    function accepts two arguments: the painter and a pointer to the source
    (QGraphicsEffectSource). The source provides extra context information,
    such as a pointer to the item that is rendering the effect, any cached
    pixmap data, or the device rectangle bounds. For more information, refer to
    the documenation for draw(). To obtain a pointer to the current source,
    simply call source().

    If your effect changes, use update() to request for a redraw. If your
    custom effect changes the bounding rectangle of the source, e.g., a radial
    glow effect may need to apply an extra margin, you can reimplement the
    virtual boundingRectFor() function, and call updateBoundingRect() to notify
    the framework whenever this rectangle changes. The virtual
    sourceBoundingRectChanged() function is called to notify the effects that
    the source's bounding rectangle has changed - e.g., if the source is a
    QGraphicsRectItem and its rectangle parameters have changed.

    \sa QGraphicsItem::setGraphicsEffect(), QWidget::setGraphicsEffect(),
    QGraphicsEffectSource
*/

#include "qgraphicseffect_p.h"

#include <QtGui/qimage.h>
#include <QtGui/qpainter.h>
#include <QtCore/qrect.h>
#include <QtCore/qdebug.h>
#include <private/qdrawhelper_p.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGraphicsEffectSource
    \brief The QGraphicsEffectSource class represents the source on which a
           QGraphicsEffect is installed on.
    \since 4.6

    When a QGraphicsEffect is installed on a QGraphicsItem, for example, this
    class will act as a wrapper around QGraphicsItem. Then, calling update() is
    effectively the same as calling QGraphicsItem::update().

    QGraphicsEffectSource also provides a pixmap() function which creates a
    pixmap with the source painted into it.

    \sa QGraphicsItem::setGraphicsEffect(), QWidget::setGraphicsEffect().
*/

/*!
    \internal
*/
QGraphicsEffectSource::QGraphicsEffectSource(QGraphicsEffectSourcePrivate &dd, QObject *parent)
    : QObject(dd, parent)
{}

/*!
    Destroys the effect source.
*/
QGraphicsEffectSource::~QGraphicsEffectSource()
{}

/*!
    Returns the bounds of the current painter's device.

    This function is useful when you want to draw something in device
    coordinates and ensure the size of the pixmap is not bigger than the size
    of the device.

    Calling QGraphicsEffectSource::pixmap(Qt::DeviceCoordinates) always returns
    a pixmap which is bound to the device's size.

    \sa pixmap()
*/
QRect QGraphicsEffectSource::deviceRect() const
{
    return d_func()->deviceRect();
}

/*!
    Returns the bounding rectangle of the source mapped to the given \a system.

    \sa draw()
*/
QRectF QGraphicsEffectSource::boundingRect(Qt::CoordinateSystem system) const
{
    return d_func()->boundingRect(system);
}

/*!
    Returns a pointer to the item if this source is a QGraphicsItem; otherwise
    returns 0.

    \sa widget()
*/
const QGraphicsItem *QGraphicsEffectSource::graphicsItem() const
{
    return d_func()->graphicsItem();
}

/*!
    Returns a pointer to the widget if this source is a QWidget; otherwise
    returns 0.

    \sa graphicsItem()
*/
const QWidget *QGraphicsEffectSource::widget() const
{
    return d_func()->widget();
}

/*!
    Returns a pointer to the style options (used when drawing the source) if
    available; otherwise returns 0.

    \sa graphicsItem(), widget()
*/
const QStyleOption *QGraphicsEffectSource::styleOption() const
{
    return d_func()->styleOption();
}

/*!
    Draws the source using the given \a painter.

    This function should only be called from QGraphicsEffect::draw().

    For example:

    \snippet doc/src/snippets/code/src_gui_effects_qgraphicseffect.cpp 0

    \sa QGraphicsEffect::draw()
*/
void QGraphicsEffectSource::draw(QPainter *painter)
{
    d_func()->draw(painter);
}

/*!
    Schedules a redraw of the source. Call this function whenever the source
    needs to be redrawn.

    \sa QGraphicsEffect::updateBoundingRect(), QWidget::update(),
        QGraphicsItem::update(),
*/
void QGraphicsEffectSource::update()
{
    d_func()->update();
}

/*!
    Returns true if the source effectively is a pixmap, e.g., a
    QGraphicsPixmapItem.

    This function is useful for optimization purposes. For instance, there's no
    point in drawing the source in device coordinates to avoid pixmap scaling
    if this function returns true - the source pixmap will be scaled anyways.
*/
bool QGraphicsEffectSource::isPixmap() const
{
    return d_func()->isPixmap();
}

/*!
    Returns a pixmap with the source painted into it.

    The \a system specifies which coordinate system to be used for the source.
    The optional \a offset parameter returns the offset where the pixmap should
    be painted at using the current painter.

    The returned pixmap is bound to the current painter's device rectangle when
    \a system is Qt::DeviceCoordinates.

    \sa QGraphicsEffect::draw(), boundingRect(), deviceRect()
*/
QPixmap QGraphicsEffectSource::pixmap(Qt::CoordinateSystem system, QPoint *offset) const
{
    Q_D(const QGraphicsEffectSource);

    QPixmap pm;
    if (d->m_cachedSystem == system)
        QPixmapCache::find(d->m_cacheKey, &pm);

    if (pm.isNull()) {
        pm = d->pixmap(system, &d->m_cachedOffset);
        d->m_cachedSystem = system;

        d->invalidateCache();
        d->m_cacheKey = QPixmapCache::insert(pm);
    }

    if (offset)
        *offset = d->m_cachedOffset;

    return pm;
}

/*!
    Constructs a new QGraphicsEffect instance having the
    specified \a parent.
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
    Returns the bounding rectangle for this effect, i.e., the bounding
    rectangle of the source, adjusted by any margins applied by the effect
    itself.

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
    no interference from the effect. If the effect is enabled, the source will
    be rendered with the effect applied.

    This property is enabled by default.

    Using this property, you can disable certain effects on slow platforms, in
    order to ensure that the user interface is responsive.
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
        d->source->d_func()->effectBoundingRectChanged();
    emit enabledChanged(enable);
}

/*!
    \fn void QGraphicsEffect::enabledChanged(bool enabled)

    This signal is emitted whenever the effect is enabled or disabled.
    The \a enabled parameter holds the effects's new enabled state.

    \sa isEnabled()
*/

/*!
    Schedules a redraw of the source. Call this function whenever the source
    needs to be redrawn.

    This convenience function is equivalent to calling
    QGraphicsEffectSource::update().

    \sa updateBoundingRect(), QGraphicsEffectSource::update()
*/
void QGraphicsEffect::update()
{
    Q_D(QGraphicsEffect);
    if (d->source)
        d->source->update();
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
    This function notifies the effect framework when the effect's bounding
    rectangle has changed. As a custom effect author, you must call this
    function whenever you change any parameters that will cause the virtual
    boundingRectFor() function to return a different value.

    This function will call update() if this is necessary.

    \sa boundingRectFor(), boundingRect()
*/
void QGraphicsEffect::updateBoundingRect()
{
    Q_D(QGraphicsEffect);
    if (d->source)
        d->source->d_func()->effectBoundingRectChanged();
}

/*!
    \fn virtual void QGraphicsEffect::draw(QPainter *painter,
    QGraphicsEffectSource *source) = 0

    This pure virtual function draws the effect and is called whenever the
    source() needs to be drawn.

    Reimplement this function in a QGraphicsEffect subclass to provide the
    effect's drawing implementation, using \a painter. The \a source parameter
    is provided for convenience; its value is the same as source().

    For example:

    \snippet doc/src/snippets/code/src_gui_effects_qgraphicseffect.cpp 1

    This function should not be called explicitly by the user, since it is
    meant for reimplementation purposes only.

    \sa QGraphicsEffectSource
*/

/*!
    \enum QGraphicsEffect::ChangeFlag

    This enum describes what has changed in QGraphicsEffectSource.

    \value SourceAttached The effect is installed on a source.
    \value SourceDetached The effect is uninstalled on a source.
    \value SourceBoundingRectChanged The bounding rect of the source has
           changed.
    \value SourceInvalidated The visual appearance of the source has changed.
*/

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

/*!
    \class QGraphicsGrayscaleEffect
    \brief The QGraphicsGrayscaleEffect class provides a grayscale effect.
    \since 4.6

    A grayscale effect renders the source in shades of gray.

    \img graphicseffect-grayscale.png

    \sa QGraphicsDropShadowEffect, QGraphicsBlurEffect, QGraphicsPixelizeEffect,
        QGraphicsColorizeEffect, QGraphicsOpacityEffect
*/

/*!
    Constructs a new QGraphicsGrayscale instance.
    The \a parent parameter is passed to QGraphicsEffect's constructor.
*/
QGraphicsGrayscaleEffect::QGraphicsGrayscaleEffect(QObject *parent)
    : QGraphicsEffect(*new QGraphicsGrayscaleEffectPrivate, parent)
{
}

/*!
    Destroys the effect.
*/
QGraphicsGrayscaleEffect::~QGraphicsGrayscaleEffect()
{
}


/*!
    \property QGraphicsGrayscaleEffect::strength
    \brief the strength of the effect.

    By default, the strength is 1.0.
    A strength 0.0 equals to no effect, while 1.0 means full grayscale.
*/
qreal QGraphicsGrayscaleEffect::strength() const
{
    Q_D(const QGraphicsGrayscaleEffect);
    return d->filter->strength();
}

void QGraphicsGrayscaleEffect::setStrength(qreal strength)
{
    Q_D(QGraphicsGrayscaleEffect);
    if (qFuzzyCompare(d->filter->strength(), strength))
        return;

    d->filter->setStrength(strength);
    d->opaque = !qFuzzyIsNull(strength);
    update();
    emit strengthChanged(strength);
}

/*! \fn void QGraphicsGrayscaleEffect::strengthChanged(qreal strength)
  This signal is emitted whenever setStrength() changes the grayscale
  strength property. \a strength contains the new strength value of
  the grayscale effect.
 */

/*!
    \reimp
*/
void QGraphicsGrayscaleEffect::draw(QPainter *painter, QGraphicsEffectSource *source)
{
    Q_D(QGraphicsGrayscaleEffect);

    if (!d->opaque) {
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

    // Draw pixmap in device coordinates to avoid pixmap scaling;
    const QPixmap pixmap = source->pixmap(Qt::DeviceCoordinates, &offset);
    QTransform restoreTransform = painter->worldTransform();
    painter->setWorldTransform(QTransform());
    d->filter->draw(painter, offset, pixmap);
    painter->setWorldTransform(restoreTransform);

}

/*!
    \class QGraphicsColorizeEffect
    \brief The QGraphicsColorizeEffect class provides a colorize effect.
    \since 4.6

    A colorize effect renders the source with a tint of its color(). The color
    can be modified using the setColor() function.

    By default, the color is light blue (QColor(0, 0, 192)).

    \img graphicseffect-colorize.png

    \sa QGraphicsDropShadowEffect, QGraphicsBlurEffect, QGraphicsPixelizeEffect,
        QGraphicsGrayscaleEffect, QGraphicsOpacityEffect
*/

/*!
    Constructs a new QGraphicsColorizeEffect instance.
    The \a parent parameter is passed to QGraphicsEffect's constructor.
*/
QGraphicsColorizeEffect::QGraphicsColorizeEffect(QObject *parent)
    : QGraphicsEffect(*new QGraphicsColorizeEffectPrivate, parent)
{
}

/*!
    Destroys the effect.
*/
QGraphicsColorizeEffect::~QGraphicsColorizeEffect()
{
}

/*!
    \property QGraphicsColorizeEffect::color
    \brief the color of the effect.

    By default, the color is light blue (QColor(0, 0, 192)).
*/
QColor QGraphicsColorizeEffect::color() const
{
    Q_D(const QGraphicsColorizeEffect);
    return d->filter->color();
}

void QGraphicsColorizeEffect::setColor(const QColor &color)
{
    Q_D(QGraphicsColorizeEffect);
    if (d->filter->color() == color)
        return;

    d->filter->setColor(color);
    update();
    emit colorChanged(color);
}

/*!
    \property QGraphicsColorizeEffect::strength
    \brief the strength of the effect.

    By default, the strength is 1.0.
    A strength 0.0 equals to no effect, while 1.0 means full colorization.
*/
qreal QGraphicsColorizeEffect::strength() const
{
    Q_D(const QGraphicsColorizeEffect);
    return d->filter->strength();
}

void QGraphicsColorizeEffect::setStrength(qreal strength)
{
    Q_D(QGraphicsColorizeEffect);
    if (qFuzzyCompare(d->filter->strength(), strength))
        return;

    d->filter->setStrength(strength);
    d->opaque = !qFuzzyIsNull(strength);
    update();
    emit strengthChanged(strength);
}

/*! \fn void QGraphicsColorizeEffect::strengthChanged(qreal strength)
  This signal is emitted whenever setStrength() changes the colorize
  strength property. \a strength contains the new strength value of
  the colorize effect.
 */

/*!
    \fn void QGraphicsColorizeEffect::colorChanged(const QColor &color)

    This signal is emitted whenever the effect's color changes.
    The \a color parameter holds the effect's new color.
*/

/*!
    \reimp
*/
void QGraphicsColorizeEffect::draw(QPainter *painter, QGraphicsEffectSource *source)
{
    Q_D(QGraphicsColorizeEffect);

    if (!d->opaque) {
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

    // Draw pixmap in deviceCoordinates to avoid pixmap scaling.
    const QPixmap pixmap = source->pixmap(Qt::DeviceCoordinates, &offset);
    QTransform restoreTransform = painter->worldTransform();
    painter->setWorldTransform(QTransform());
    d->filter->draw(painter, offset, pixmap);
    painter->setWorldTransform(restoreTransform);
}

/*!
    \class QGraphicsPixelizeEffect
    \brief The QGraphicsPixelizeEffect class provides a pixelize effect.
    \since 4.6

    A pixelize effect renders the source in lower resolution. This effect is
    useful for reducing details, like censorship. The resolution can be
    modified using the setPixelSize() function.

    By default, the pixel size is 3.

    \img graphicseffect-pixelize.png

    \sa QGraphicsDropShadowEffect, QGraphicsBlurEffect, QGraphicsGrayscaleEffect,
        QGraphicsColorizeEffect, QGraphicsOpacityEffect
*/

/*!
    Constructs a new QGraphicsPixelizeEffect instance.
    The \a parent parameter is passed to QGraphicsEffect's constructor.
*/
QGraphicsPixelizeEffect::QGraphicsPixelizeEffect(QObject *parent)
    : QGraphicsEffect(*new QGraphicsPixelizeEffectPrivate, parent)
{
}

/*!
    Destroys the effect.
*/
QGraphicsPixelizeEffect::~QGraphicsPixelizeEffect()
{
}

/*!
    \property QGraphicsPixelizeEffect::pixelSize
    \brief the size of a pixel in the effect.

    Setting the pixel size to 2 means two pixels in the source will be used to
    represent one pixel. Using a bigger size results in lower resolution.

    By default, the pixel size is 3.
*/
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
    update();
    emit pixelSizeChanged(size);
}

/*!
    \fn void QGraphicsPixelizeEffect::pixelSizeChanged(int size)

    This signal is emitted whenever the effect's pixel size changes.
    The \a size parameter holds the effect's new pixel size.
*/

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

/*!
    \reimp
*/
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

/*!
    \class QGraphicsBlurEffect
    \brief The QGraphicsBlurEffect class provides a blur effect.
    \since 4.6

    A blur effect blurs the source. This effect is useful for reducing details,
    such as when the source loses focus and you want to draw attention to other
    elements. The level of detail can be modified using the setBlurRadius()
    function. Use setBlurHint() to choose the quality or performance blur hints.

    By default, the blur radius is 5 pixels.

    \img graphicseffect-blur.png

    \sa QGraphicsDropShadowEffect, QGraphicsPixelizeEffect, QGraphicsGrayscaleEffect,
        QGraphicsColorizeEffect, QGraphicsOpacityEffect
*/

/*!
    Constructs a new QGraphicsBlurEffect instance.
    The \a parent parameter is passed to QGraphicsEffect's constructor.
*/
QGraphicsBlurEffect::QGraphicsBlurEffect(QObject *parent)
    : QGraphicsEffect(*new QGraphicsBlurEffectPrivate, parent)
{
    Q_D(QGraphicsBlurEffect);
    d->filter->setBlurHint(Qt::PerformanceHint);
}

/*!
    Destroys the effect.
*/
QGraphicsBlurEffect::~QGraphicsBlurEffect()
{
}

/*!
    \property QGraphicsBlurEffect::blurRadius
    \brief the blur radius of the effect.

    Using a smaller radius results in a sharper appearance, whereas a bigger
    radius results in a more blurred appearance.

    By default, the blur radius is 5 pixels.
*/
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

/*!
    \fn void QGraphicsBlurEffect::blurRadiusChanged(int radius)

    This signal is emitted whenever the effect's blur radius changes.
    The \a radius parameter holds the effect's new blur radius.
*/

/*!
    \property QGraphicsBlurEffect::blurHint
    \brief the blur hint of the effect.

    Use the Qt::PerformanceHint hint to say that you want a faster blur,
    and the Qt::QualityHint hint to say that you prefer a higher quality blur.

    When animating the blur radius it's recommended to use Qt::PerformanceHint.

    By default, the blur hint is Qt::PerformanceHint.
*/
Qt::RenderHint QGraphicsBlurEffect::blurHint() const
{
    Q_D(const QGraphicsBlurEffect);
    return d->filter->blurHint();
}

void QGraphicsBlurEffect::setBlurHint(Qt::RenderHint hint)
{
    Q_D(QGraphicsBlurEffect);
    if (d->filter->blurHint() == hint)
        return;

    d->filter->setBlurHint(hint);
    emit blurHintChanged(hint);
}

/*!
    \fn void QGraphicsBlurEffect::blurHintChanged(Qt::RenderHint hint)

    This signal is emitted whenever the effect's blur hint changes.
    The \a hint parameter holds the effect's new blur hint.
*/

/*!
    \reimp
*/
QRectF QGraphicsBlurEffect::boundingRectFor(const QRectF &rect) const
{
    Q_D(const QGraphicsBlurEffect);
    return d->filter->boundingRectFor(rect);
}

/*!
    \reimp
*/
void QGraphicsBlurEffect::draw(QPainter *painter, QGraphicsEffectSource *source)
{
    Q_D(QGraphicsBlurEffect);
    if (d->filter->radius() <= 0) {
        source->draw(painter);
        return;
    }

    // Draw pixmap in device coordinates to avoid pixmap scaling.
    QPoint offset;
    const QPixmap pixmap = source->pixmap(Qt::DeviceCoordinates, &offset);
    QTransform restoreTransform = painter->worldTransform();
    painter->setWorldTransform(QTransform());
    d->filter->draw(painter, offset, pixmap);
    painter->setWorldTransform(restoreTransform);
}

/*!
    \class QGraphicsDropShadowEffect
    \brief The QGraphicsDropShadowEffect class provides a drop shadow effect.
    \since 4.6

    A drop shadow effect renders the source with a drop shadow. The color of
    the drop shadow can be modified using the setColor() function. The drop
    shadow offset can be modified using the setOffset() function and the blur
    radius of the drop shadow can be changed with the setBlurRadius()
    function.

    By default, the drop shadow is a semi-transparent dark gray
    (QColor(63, 63, 63, 180)) shadow, blurred with a radius of 1 at an offset
    of 8 pixels towards the lower right.

    \img graphicseffect-drop-shadow.png

    \sa QGraphicsBlurEffect, QGraphicsPixelizeEffect, QGraphicsGrayscaleEffect,
        QGraphicsColorizeEffect, QGraphicsOpacityEffect
*/

/*!
    Constructs a new QGraphicsDropShadowEffect instance.
    The \a parent parameter is passed to QGraphicsEffect's constructor.
*/
QGraphicsDropShadowEffect::QGraphicsDropShadowEffect(QObject *parent)
    : QGraphicsEffect(*new QGraphicsDropShadowEffectPrivate, parent)
{
}

/*!
    Destroys the effect.
*/
QGraphicsDropShadowEffect::~QGraphicsDropShadowEffect()
{
}

/*!
    \property QGraphicsDropShadowEffect::offset
    \brief the shadow offset in pixels.

    By default, the offset is 8 pixels towards the lower right.

    \sa xOffset(), yOffset(), blurRadius(), color()
*/
QPointF QGraphicsDropShadowEffect::offset() const
{
    Q_D(const QGraphicsDropShadowEffect);
    return d->filter->offset();
}

void QGraphicsDropShadowEffect::setOffset(const QPointF &offset)
{
    Q_D(QGraphicsDropShadowEffect);
    if (d->filter->offset() == offset)
        return;

    d->filter->setOffset(offset);
    updateBoundingRect();
    emit offsetChanged(offset);
}

/*!
    \property QGraphicsDropShadowEffect::xOffset
    \brief the horizontal shadow offset in pixels.

    By default, the horizontal shadow offset is 8 pixels.

    \sa yOffset(), offset()
*/

/*!
    \property QGraphicsDropShadowEffect::yOffset
    \brief the vertical shadow offset in pixels.

    By default, the vertical shadow offset is 8 pixels.

    \sa xOffset(), offset()
*/

/*!
    \fn void QGraphicsDropShadowEffect::offsetChanged(const QPointF &offset)

    This signal is emitted whenever the effect's shadow offset changes.
    The \a offset parameter holds the effect's new shadow offset.
*/

/*!
    \property QGraphicsDropShadowEffect::blurRadius
    \brief the blur radius in pixels of the drop shadow.

    Using a smaller radius results in a sharper shadow, whereas using a bigger
    radius results in a more blurred shadow.

    By default, the blur radius is 1 pixel.

    \sa color(), offset().
*/
int QGraphicsDropShadowEffect::blurRadius() const
{
    Q_D(const QGraphicsDropShadowEffect);
    return d->filter->blurRadius();
}

void QGraphicsDropShadowEffect::setBlurRadius(int blurRadius)
{
    Q_D(QGraphicsDropShadowEffect);
    if (d->filter->blurRadius() == blurRadius)
        return;

    d->filter->setBlurRadius(blurRadius);
    updateBoundingRect();
    emit blurRadiusChanged(blurRadius);
}

/*!
    \fn void QGraphicsDropShadowEffect::blurRadiusChanged(int blurRadius)

    This signal is emitted whenever the effect's blur radius changes.
    The \a blurRadius parameter holds the effect's new blur radius.
*/

/*!
    \property QGraphicsDropShadowEffect::color
    \brief the color of the drop shadow.

    By default, the drop color is a semi-transparent dark gray
    (QColor(63, 63, 63, 180)).

    \sa offset(), blurRadius()
*/
QColor QGraphicsDropShadowEffect::color() const
{
    Q_D(const QGraphicsDropShadowEffect);
    return d->filter->color();
}

void QGraphicsDropShadowEffect::setColor(const QColor &color)
{
    Q_D(QGraphicsDropShadowEffect);
    if (d->filter->color() == color)
        return;

    d->filter->setColor(color);
    update();
    emit colorChanged(color);
}

/*!
    \fn void QGraphicsDropShadowEffect::colorChanged(const QColor &color)

    This signal is emitted whenever the effect's color changes.
    The \a color parameter holds the effect's new color.
*/

/*!
    \reimp
*/
QRectF QGraphicsDropShadowEffect::boundingRectFor(const QRectF &rect) const
{
    Q_D(const QGraphicsDropShadowEffect);
    return d->filter->boundingRectFor(rect);
}

/*!
    \reimp
*/
void QGraphicsDropShadowEffect::draw(QPainter *painter, QGraphicsEffectSource *source)
{
    Q_D(QGraphicsDropShadowEffect);
    if (d->filter->blurRadius() <= 0 && d->filter->offset().isNull()) {
        source->draw(painter);
        return;
    }

    // Draw pixmap in device coordinates to avoid pixmap scaling.
    QPoint offset;
    const QPixmap pixmap = source->pixmap(Qt::DeviceCoordinates, &offset);
    QTransform restoreTransform = painter->worldTransform();
    painter->setWorldTransform(QTransform());
    d->filter->draw(painter, offset, pixmap);
    painter->setWorldTransform(restoreTransform);
}

/*!
    \class QGraphicsOpacityEffect
    \brief The QGraphicsOpacityEffect class provides an opacity effect.
    \since 4.6

    An opacity effect renders the source with an opacity. This effect is useful
    for making the source semi-transparent, similar to a fade-in/fade-out
    sequence. The opacity can be modified using the setOpacity() function.

    By default, the opacity is 0.7.

    \img graphicseffect-opacity.png

    \sa QGraphicsDropShadowEffect, QGraphicsBlurEffect, QGraphicsPixelizeEffect,
        QGraphicsGrayscaleEffect, QGraphicsColorizeEffect
*/

/*!
    Constructs a new QGraphicsOpacityEffect instance.
    The \a parent parameter is passed to QGraphicsEffect's constructor.
*/
QGraphicsOpacityEffect::QGraphicsOpacityEffect(QObject *parent)
    : QGraphicsEffect(*new QGraphicsOpacityEffectPrivate, parent)
{
}

/*!
    Destroys the effect.
*/
QGraphicsOpacityEffect::~QGraphicsOpacityEffect()
{
}

/*!
    \property QGraphicsOpacityEffect::opacity
    \brief the opacity of the effect.

    The value should be in the range of 0.0 to 1.0, where 0.0 is
    fully transparent and 1.0 is fully opaque.

    By default, the opacity is 0.7.

    \sa setOpacityMask()
*/
qreal QGraphicsOpacityEffect::opacity() const
{
    Q_D(const QGraphicsOpacityEffect);
    return d->opacity;
}

void QGraphicsOpacityEffect::setOpacity(qreal opacity)
{
    Q_D(QGraphicsOpacityEffect);
    opacity = qBound(qreal(0.0), opacity, qreal(1.0));

    if (qFuzzyCompare(d->opacity, opacity))
        return;

    d->opacity = opacity;
    if ((d->isFullyTransparent = qFuzzyIsNull(d->opacity)))
        d->isFullyOpaque = 0;
    else
        d->isFullyOpaque = qFuzzyIsNull(d->opacity - 1);
    update();
    emit opacityChanged(opacity);
}

/*!
    \fn void QGraphicsOpacityEffect::opacityChanged(qreal opacity)

    This signal is emitted whenever the effect's opacity changes.
    The \a opacity parameter holds the effect's new opacity.
*/

/*!
    \property QGraphicsOpacityEffect::opacityMask
    \brief the opacity mask of the effect.

    An opacity mask allows you apply opacity to portions of an element.

    For example:

    \snippet doc/src/snippets/code/src_gui_effects_qgraphicseffect.cpp 2

    There is no opacity mask by default.

    \sa setOpacity()
*/
QBrush QGraphicsOpacityEffect::opacityMask() const
{
    Q_D(const QGraphicsOpacityEffect);
    return d->opacityMask;
}

void QGraphicsOpacityEffect::setOpacityMask(const QBrush &mask)
{
    Q_D(QGraphicsOpacityEffect);
    if (d->opacityMask == mask)
        return;

    d->opacityMask = mask;
    d->hasOpacityMask = (mask.style() != Qt::NoBrush);
    update();

    emit opacityMaskChanged(mask);
}

/*!
    \fn void QGraphicsOpacityEffect::opacityMaskChanged(const QBrush &mask)

    This signal is emitted whenever the effect's opacity mask changes.
    The \a mask parameter holds the effect's new opacity mask.
*/

/*!
    \reimp
*/
void QGraphicsOpacityEffect::draw(QPainter *painter, QGraphicsEffectSource *source)
{
    Q_D(QGraphicsOpacityEffect);

    // Transparent; nothing to draw.
    if (d->isFullyTransparent)
        return;

    // Opaque; draw directly without going through a pixmap.
    if (d->isFullyOpaque && !d->hasOpacityMask) {
        source->draw(painter);
        return;
    }

    painter->save();
    painter->setOpacity(d->opacity);

    QPoint offset;
    if (source->isPixmap()) {
        // No point in drawing in device coordinates (pixmap will be scaled anyways).
        if (!d->hasOpacityMask) {
            const QPixmap pixmap = source->pixmap(Qt::LogicalCoordinates, &offset);
            painter->drawPixmap(offset, pixmap);
        } else {
            QRect srcBrect = source->boundingRect().toAlignedRect();
            offset = srcBrect.topLeft();
            QPixmap pixmap(srcBrect.size());
            pixmap.fill(Qt::transparent);

            QPainter pixmapPainter(&pixmap);
            pixmapPainter.setRenderHints(painter->renderHints());
            pixmapPainter.translate(-offset);
            source->draw(&pixmapPainter);
            pixmapPainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            pixmapPainter.fillRect(srcBrect, d->opacityMask);
            pixmapPainter.end();

            painter->drawPixmap(offset, pixmap);
        }
    } else {
        // Draw pixmap in device coordinates to avoid pixmap scaling;
        if (!d->hasOpacityMask) {
            const QPixmap pixmap = source->pixmap(Qt::DeviceCoordinates, &offset);
            painter->setWorldTransform(QTransform());
            painter->drawPixmap(offset, pixmap);
        } else {
            QTransform worldTransform = painter->worldTransform();

            // Calculate source bounding rect in logical and device coordinates.
            QRectF srcBrect = source->boundingRect();
            QRect srcDeviceBrect = worldTransform.mapRect(srcBrect).toAlignedRect();
            srcDeviceBrect &= source->deviceRect();

            offset = srcDeviceBrect.topLeft();
            worldTransform *= QTransform::fromTranslate(-srcDeviceBrect.x(), -srcDeviceBrect.y());

            QPixmap pixmap(srcDeviceBrect.size());
            pixmap.fill(Qt::transparent);

            QPainter pixmapPainter(&pixmap);
            pixmapPainter.setRenderHints(painter->renderHints());
            pixmapPainter.setWorldTransform(worldTransform);
            source->draw(&pixmapPainter);
            pixmapPainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            pixmapPainter.fillRect(srcBrect, d->opacityMask);
            pixmapPainter.end();

            painter->setWorldTransform(QTransform());
            painter->drawPixmap(offset, pixmap);
        }
    }

    painter->restore();
}

/*!
    \class QGraphicsBloomEffect
    \brief The QGraphicsBloomEffect class provides a bloom/glow effect.
    \since 4.6

    A bloom/glow effect adds fringes of light around bright areas in the source.

    \img graphicseffect-bloom.png

    \sa QGraphicsDropShadowEffect, QGraphicsBlurEffect, QGraphicsPixelizeEffect,
        QGraphicsGrayscaleEffect, QGraphicsColorizeEffect
*/

/*!
    Constructs a new QGraphicsBloomEffect instance.
    The \a parent parameter is passed to QGraphicsEffect's constructor.
*/
QGraphicsBloomEffect::QGraphicsBloomEffect(QObject *parent)
    : QGraphicsEffect(*new QGraphicsBloomEffectPrivate, parent)
{
    Q_D(QGraphicsBloomEffect);
    for (int i = 0; i < 256; ++i)
        d->colorTable[i] = qMin(i + d->brightness, 255);
}

/*!
    Destroys the effect.
*/
QGraphicsBloomEffect::~QGraphicsBloomEffect()
{
}

/*!
    \reimp
*/
QRectF QGraphicsBloomEffect::boundingRectFor(const QRectF &rect) const
{
    Q_D(const QGraphicsBloomEffect);
    const qreal delta = d->blurFilter.radius() * 2;
    return rect.adjusted(-delta, -delta, delta, delta);
}

/*!
    \property QGraphicsBloomEffect::blurRadius
    \brief the blur radius in pixels of the effect.

    Using a smaller radius results in a sharper appearance, whereas a bigger
    radius results in a more blurred appearance.

    By default, the blur radius is 5 pixels.

    \sa strength(), brightness()
*/
int QGraphicsBloomEffect::blurRadius() const
{
    Q_D(const QGraphicsBloomEffect);
    return d->blurFilter.radius();
}

void QGraphicsBloomEffect::setBlurRadius(int radius)
{
    Q_D(QGraphicsBloomEffect);
    if (d->blurFilter.radius() == radius)
        return;

    d->blurFilter.setRadius(radius);
    updateBoundingRect();
    emit blurRadiusChanged(radius);
}

/*!
    \fn void QGraphicsBloomEffect::blurRadiusChanged(int blurRadius)

    This signal is emitted whenever the effect's blur radius changes.
    The \a blurRadius parameter holds the effect's new blur radius.
*/

/*!
    \property QGraphicsBloomEffect::blurHint
    \brief the blur hint of the effect.

    Use the Qt::PerformanceHint hint to say that you want a faster blur,
    and the Qt::QualityHint hint to say that you prefer a higher quality blur.

    When animating the blur radius it's recommended to use Qt::PerformanceHint.

    By default, the blur hint is Qt::PerformanceHint.
*/
Qt::RenderHint QGraphicsBloomEffect::blurHint() const
{
    Q_D(const QGraphicsBloomEffect);
    return d->blurFilter.blurHint();
}

void QGraphicsBloomEffect::setBlurHint(Qt::RenderHint hint)
{
    Q_D(QGraphicsBloomEffect);
    if (d->blurFilter.blurHint() == hint)
        return;

    d->blurFilter.setBlurHint(hint);
    emit blurHintChanged(hint);
}

/*!
    \fn void QGraphicsBloomEffect::blurHintChanged(Qt::RenderHint hint)

    This signal is emitted whenever the effect's blur hint changes.
    The \a hint parameter holds the effect's new blur hint.
*/

/*!
    \property QGraphicsBloomEffect::brightness
    \brief the brightness of the glow.

    The value should be in the range of 0 to 255, where 0 is dark
    and 255 is bright.

    By default, the brightness is 70.

    \sa strength(), blurRadius()
*/
int QGraphicsBloomEffect::brightness() const
{
    Q_D(const QGraphicsBloomEffect);
    return d->brightness;
}

void QGraphicsBloomEffect::setBrightness(int brightness)
{
    Q_D(QGraphicsBloomEffect);
    brightness = qBound(0, brightness, 255);
    if (d->brightness == brightness)
        return;

    d->brightness = brightness;
    for (int i = 0; i < 256; ++i)
        d->colorTable[i] = qMin(i + brightness, 255);

    update();
    emit brightnessChanged(brightness);
}

/*!
    \fn void QGraphicsBloomEffect::brightnessChanged(int brightness)

    This signal is emitted whenever the effect's brightness changes.
    The \a brightness parameter holds the effect's new brightness.
*/

/*!
    \property QGraphicsBloomEffect::strength
    \brief the strength of the effect.

    A strength 0.0 equals to no effect, while 1.0 means maximum glow.

    By default, the strength is 0.7.
*/
qreal QGraphicsBloomEffect::strength() const
{
    Q_D(const QGraphicsBloomEffect);
    return d->strength;
}

void QGraphicsBloomEffect::setStrength(qreal strength)
{
    Q_D(QGraphicsBloomEffect);
    strength = qBound(qreal(0.0), strength, qreal(1.0));
    if (qFuzzyCompare(d->strength, strength))
        return;

    d->strength = strength;
    update();
    emit strengthChanged(strength);
}

/*!
    \fn void QGraphicsBloomEffect::strengthChanged(qreal strength)

    This signal is emitted whenever the effect's strength changes.
    The \a strength parameter holds the effect's new strength.
*/

extern QPixmap qt_toRasterPixmap(const QPixmap &pixmap);

/*!
    \reimp
*/
void QGraphicsBloomEffect::draw(QPainter *painter, QGraphicsEffectSource *source)
{
    Q_D(QGraphicsBloomEffect);
    if (d->strength < 0.001) {
        source->draw(painter);
        return;
    }

    QPoint offset;
    QPixmap pixmap = qt_toRasterPixmap(source->pixmap(Qt::DeviceCoordinates, &offset));

    // Blur.
    QImage overlay(pixmap.size(), QImage::Format_ARGB32_Premultiplied);
    overlay.fill(0);

    QPainter blurPainter(&overlay);
    d->blurFilter.draw(&blurPainter, QPointF(), pixmap);
    blurPainter.end();

    // Brighten.
    const int numBits = overlay.width() * overlay.height();
    QRgb *bits = reinterpret_cast<QRgb *>(overlay.bits());
    for (int i = 0; i < numBits; ++i) {
        const QRgb pixel = INV_PREMUL(bits[i]);
        bits[i] = PREMUL(qRgba(d->colorTable[qRed(pixel)], d->colorTable[qGreen(pixel)],
                               d->colorTable[qBlue(pixel)], qAlpha(pixel)));
    }

    // Composite.
    QPainter compPainter(&pixmap);
    compPainter.setCompositionMode(QPainter::CompositionMode_Overlay);
    compPainter.setOpacity(d->strength);
    compPainter.drawImage(0, 0, overlay);
    compPainter.end();

    QTransform restoreTransform = painter->worldTransform();
    painter->setWorldTransform(QTransform());
    painter->drawPixmap(offset, pixmap);
    painter->setWorldTransform(restoreTransform);
}

QT_END_NAMESPACE

