/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

/*!
    \class QGraphicsTransform
    \brief The QGraphicsTransform class is an abstract base class for building
    advanced transformations on QGraphicsItems.
    \since 4.6

    As an alternative to QGraphicsItem::transform, QGraphicsTransform lets you
    create and control advanced transformations that can be configured
    independently using specialized properties.

    QGraphicsItem allows you to assign any number of QGraphicsTransform
    instances to one QGraphicsItem. Each QGraphicsTransform is applied in
    order, one at a time, to the QGraphicsItem it's assigned to.

    QGraphicsTransform is particularily useful for animations. Whereas
    QGraphicsItem::setTransform() lets you assign any transform directly to an
    item, there is no direct way to interpolate between two different
    transformations (e.g., when transitioning between two states, each for
    which the item has a different arbitrary transform assigned). Using
    QGraphicsTransform you can interpolate the property values of each
    independent transformation. The resulting operation is then combined into a
    single transform which is applied to QGraphicsItem.

    If you want to create your own configurable transformation, you can create
    a subclass of QGraphicsTransform (or any or the existing subclasses), and
    reimplement the pure virtual applyTo() function, which takes a pointer to a
    QTransform. Each operation you would like to apply should be exposed as
    properties (e.g., customTransform->setVerticalShear(2.5)). Inside you
    reimplementation of applyTo(), you can modify the provided transform
    respectively.

    QGraphicsTransform can be used together with QGraphicsItem::setTransform(),
    QGraphicsItem::setRotation(), and QGraphicsItem::setScale().

    \sa QGraphicsItem::transform(), QGraphicsScale, QGraphicsRotation, QGraphicsRotation3D
*/

#include "qgraphicstransform.h"
#include "qgraphicsitem_p.h"
#include "qgraphicstransform_p.h"
#include <QDebug>
#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

void QGraphicsTransformPrivate::setItem(QGraphicsItem *i)
{
    if (item == i)
        return;

    if (item) {
        Q_Q(QGraphicsTransform);
        QGraphicsItemPrivate *d_ptr = item->d_ptr;

        item->prepareGeometryChange();
        Q_ASSERT(d_ptr->transformData);
        d_ptr->transformData->graphicsTransforms.removeAll(q);
        d_ptr->dirtySceneTransform = 1;
        item = 0;
    }

    item = i;
}

void QGraphicsTransformPrivate::updateItem(QGraphicsItem *item)
{
    item->prepareGeometryChange();
    item->d_ptr->dirtySceneTransform = 1;
}

/*!
    Constructs a new QGraphicsTransform with the given \a parent.
*/
QGraphicsTransform::QGraphicsTransform(QObject *parent)
    : QObject(*new QGraphicsTransformPrivate, parent)
{
}

/*!
    Destroys the graphics transform.
*/
QGraphicsTransform::~QGraphicsTransform()
{
    Q_D(QGraphicsTransform);
    d->setItem(0);
}

/*!
    \internal
*/
QGraphicsTransform::QGraphicsTransform(QGraphicsTransformPrivate &p, QObject *parent)
    : QObject(p, parent)
{
}

/*!
    Applies this transformation to an identity transform, and returns the
    resulting transform.

    This is equivalent to passing an identity transform to applyTo().

    \sa applyTo()
*/
QTransform QGraphicsTransform::transform() const
{
    QTransform t;
    applyTo(&t);
    return t;
}

/*!
    \fn void QGraphicsTransform::applyTo(QTransform *transform) const

    This pure virtual method has to be reimplemented in derived classes.

    It applies this transformation to \a transform.

    \sa QGraphicsItem::transform()
*/

/*!
    Notifies that this transform operation has changed its parameters in such a
    way that applyTo() will return a different result than before.

    When implementing you own custom graphics transform, you must call this
    function every time you change a parameter, to let QGraphicsItem know that
    its transformation needs to be updated.

    \sa applyTo()
*/
void QGraphicsTransform::update()
{
    Q_D(QGraphicsTransform);
    if (d->item)
        d->updateItem(d->item);
}

/*!
  \class QGraphicsScale
  \brief The QGraphicsScale class provides a scale transformation.
  \since 4.6

  QGraphicsScene provides certain parameters to help control how the scale
  should be applied.

  The origin is the point that the item is scaled from (i.e., it stays fixed
  relative to the parent as the rest of the item grows). By default the
  origin is QPointF(0, 0).

  The two parameters xScale and yScale describe the scale factors to apply in
  horizontal and vertical direction. They can take on any value, including 0
  (to collapse the item to a point) or negativate value. A negative xScale
  value will mirror the item horizontally. A negative yScale value will flip
  the item vertically.

  \sa QGraphicsTransform, QGraphicsItem::setScale(), QTransform::scale()
*/

class QGraphicsScalePrivate : public QGraphicsTransformPrivate
{
public:
    QGraphicsScalePrivate()
        : xScale(1), yScale(1) {}
    QPointF origin;
    qreal xScale;
    qreal yScale;
};

/*!
    Constructs an empty QGraphicsScale object with the given \a parent.
*/
QGraphicsScale::QGraphicsScale(QObject *parent)
    : QGraphicsTransform(*new QGraphicsScalePrivate, parent)
{
}

/*!
    Destroys the graphics scale.
*/
QGraphicsScale::~QGraphicsScale()
{
}

/*!
    \property QGraphicsScale::origin
    \brief The QGraphicsScene class provides the origin of the scale.

    All scaling will be done relative to this point (i.e., this point
    will stay fixed, relative to the parent, when the item is scaled).

    \sa xScale, yScale
*/
QPointF QGraphicsScale::origin() const
{
    Q_D(const QGraphicsScale);
    return d->origin;
}
void QGraphicsScale::setOrigin(const QPointF &point)
{
    Q_D(QGraphicsScale);
    d->origin = point;
    update();
    emit originChanged();
}

/*!
    \property QGraphicsScale::xScale
    \brief the horizontal scale factor.

    The scale factor can be any real number; the default value is 1.0. If you
    set the factor to 0.0, the item will be collapsed to a single point. If you
    provide a negative value, the item will be mirrored horizontally around its
    origin.

    \sa yScale, origin
*/
qreal QGraphicsScale::xScale() const
{
    Q_D(const QGraphicsScale);
    return d->xScale;
}
void QGraphicsScale::setXScale(qreal scale)
{
    Q_D(QGraphicsScale);
    if (d->xScale == scale)
        return;
    d->xScale = scale;
    update();
    emit scaleChanged();
}

/*!
    \property QGraphicsScale::yScale
    \brief the vertical scale factor.

    The scale factor can be any real number; the default value is 1.0. If you
    set the factor to 0.0, the item will be collapsed to a single point. If you
    provide a negative value, the item will be flipped vertically around its
    origin.

    \sa xScale, origin
*/
qreal QGraphicsScale::yScale() const
{
    Q_D(const QGraphicsScale);
    return d->yScale;
}
void QGraphicsScale::setYScale(qreal scale)
{
    Q_D(QGraphicsScale);
    if (d->yScale == scale)
        return;
    d->yScale = scale;
    update();
    emit scaleChanged();
}

/*!
    \reimp
*/
void QGraphicsScale::applyTo(QTransform *transform) const
{
    Q_D(const QGraphicsScale);
    transform->translate(d->origin.x(), d->origin.y());
    transform->scale(d->xScale, d->yScale);
    transform->translate(-d->origin.x(), -d->origin.y());
}

/*!
    \fn QGraphicsScale::originChanged()

    QGraphicsScale emits this signal when its origin changes.

    \sa QGraphicsScale::origin
*/

/*!
    \fn QGraphicsScale::scaleChanged()

    This signal is emitted whenever the xScale or yScale of the object
    changes.

    \sa QGraphicsScale::xScale, QGraphicsScale::yScale
*/

/*!
    \class QGraphicsRotation
    \brief The QGraphicsRotation class provides a rotation transformation.
    \since 4.6

    QGraphicsRotation provides certain parameters to help control how the
    rotation should be applied.

    The origin is the point that the item is rotated around (i.e., it stays
    fixed relative to the parent as the rest of the item is rotated). By
    default the origin is QPointF(0, 0).

    The angle property provides the number of degrees to rotate the item
    clockwise around the origin. This value also be negative, indicating a
    counter-clockwise rotation. For animation purposes it may also be useful to
    provide rotation angles exceeding (-360, 360) degrees, for instance to
    animate how an item rotates several times.

    \sa QGraphicsTransform, QGraphicsItem::setRotation(), QTransform::rotate()
*/

class QGraphicsRotationPrivate : public QGraphicsTransformPrivate
{
public:
    QGraphicsRotationPrivate()
            : angle(0) {}
    QPointF origin;
    qreal angle;
};

/*!
    Constructs a new QGraphicsRotation with the given \a parent.
*/
QGraphicsRotation::QGraphicsRotation(QObject *parent)
    : QGraphicsTransform(*new QGraphicsRotationPrivate, parent)
{
}

/*!
    \internal
*/
QGraphicsRotation::QGraphicsRotation(QGraphicsRotationPrivate &p, QObject *parent)
    : QGraphicsTransform(p, parent)
{
}

/*!
    Destroys the graphics rotation.
*/
QGraphicsRotation::~QGraphicsRotation()
{
}

/*!
    \property QGraphicsRotation::origin
    \brief the origin of the rotation.

    All rotations will be done relative to this point (i.e., this point
    will stay fixed, relative to the parent, when the item is rotated).

    \sa angle
*/
QPointF QGraphicsRotation::origin() const
{
    Q_D(const QGraphicsRotation);
    return d->origin;
}
void QGraphicsRotation::setOrigin(const QPointF &point)
{
    Q_D(QGraphicsRotation);
    d->origin = point;
    update();
    emit originChanged();
}

/*!
    \property QGraphicsRotation::angle
    \brief the angle for clockwise rotation, in degrees.

    The angle can be any real number; the default value is 0.0. A value of 180
    will rotate 180 degrees, clockwise. If you provide a negative number, the
    item will be rotated counter-clockwise. Normally the rotation angle will be
    in the range (-360, 360), but you can also provide numbers outside of this
    range (e.g., a angle of 370 degrees gives the same result as 10 degrees).

    \sa origin
*/
qreal QGraphicsRotation::angle() const
{
    Q_D(const QGraphicsRotation);
    return d->angle;
}
void QGraphicsRotation::setAngle(qreal angle)
{
    Q_D(QGraphicsRotation);
    if (d->angle == angle)
        return;
    d->angle = angle;
    update();
    emit angleChanged();
}

/*!
    \reimp
*/
void QGraphicsRotation::applyTo(QTransform *t) const
{
    Q_D(const QGraphicsRotation);
    if (d->angle) {
        t->translate(d->origin.x(), d->origin.y());
        t->rotate(d->angle);
        t->translate(-d->origin.x(), -d->origin.y());
    }
}

/*!
    \fn QGraphicsRotation::originChanged()

    This signal is emitted whenever the origin has changed.

    \sa QGraphicsRotation::origin
*/

/*!
  \fn void QGraphicsRotation::angleChanged()

  This signal is emitted whenever the angle has changed.

  \sa QGraphicsRotation::angle
*/

/*!
    \class QGraphicsRotation3D
    \brief The QGraphicsRotation3D class provides rotation in 3 dimensions.
    \since 4.6

    QGraphicsRotation3D extends QGraphicsRotation with the ability to rotate
    around a given axis.

    You can provide the desired axis by assigning a QVector3D to the axis
    property. The angle property, which is provided by QGraphicsRotation, now
    describes the number of degrees to rotate around this axis.

    By default the axis is (0, 0, 1), giving QGraphicsRotation3D the same
    default behavior as QGraphicsRotation (i.e., rotation around the Z axis).

    Note: the final rotation is the combined effect of a rotation in
    3D space followed by a projection back to 2D.  If several rotations
    are performed in succession, they will not behave as expected unless
    they were all around the Z axis.

    \sa QGraphicsTransform, QGraphicsItem::setRotation(), QTransform::rotate()
*/

class QGraphicsRotation3DPrivate : public QGraphicsRotationPrivate
{
public:
    QGraphicsRotation3DPrivate() : axis(0, 0, 1) {}

    QVector3D axis;
};

/*!
    Constructs a new QGraphicsRotation3D with the given \a parent.
*/
QGraphicsRotation3D::QGraphicsRotation3D(QObject *parent)
    : QGraphicsRotation(*new QGraphicsRotation3DPrivate, parent)
{
}

/*!
    Destroys the 3D graphics rotation.
*/
QGraphicsRotation3D::~QGraphicsRotation3D()
{
}

/*!
    \property QGraphicsRotation3D::axis
    \brief a rotation axis, specified by a vector in 3D space.

    This can be any axis in 3D space. By default the axis is (0, 0, 1),
    which is aligned with the Z axis and provides the same behavior
    for the rotation angle as QGraphicsRotation. If you provide another
    axis, QGraphicsRotation3D will provide a transformation that rotates
    around this axis. For example, if you would like to rotate an item
    around its X axis, you could pass (1, 0, 0) as the axis.

    \sa QTransform, QGraphicsRotation::angle
*/
QVector3D QGraphicsRotation3D::axis()
{
    Q_D(QGraphicsRotation3D);
    return d->axis;
}
void QGraphicsRotation3D::setAxis(const QVector3D &axis)
{
    Q_D(QGraphicsRotation3D);
    d->axis = axis;
    update();
}

const qreal deg2rad = qreal(0.017453292519943295769);        // pi/180
static const qreal inv_dist_to_plane = 1. / 1024.;

/*!
    \reimp
*/
void QGraphicsRotation3D::applyTo(QTransform *t) const
{
    Q_D(const QGraphicsRotation3D);

    qreal a = d->angle;

    if (a == 0. ||
        (d->axis.z() == 0. && d->axis.y() == 0 && d->axis.x() == 0))
        return;

    qreal c, s;
    if (a == 90. || a == -270.) {
        s = 1.;
        c = 0.;
    } else if (a == 270. || a == -90.) {
        s = -1.;
        c = 0.;
    } else if (a == 180.) {
        s = 0.;
        c = -1.;
    } else {
        qreal b = deg2rad*a;
        s = qSin(b);
        c = qCos(b);
    }

    qreal x = d->axis.x();
    qreal y = d->axis.y();
    qreal z = d->axis.z();

    qreal len = x * x + y * y + z * z;
    if (len != 1.) {
        len = 1./::sqrt(len);
        x *= len;
        y *= len;
        z *= len;
    }

    t->translate(d->origin.x(), d->origin.y());
    *t = QTransform(x*x*(1-c)+c, x*y*(1-c)-z*s, x*z*(1-c)+y*s*inv_dist_to_plane,
                             y*x*(1-c)+z*s, y*y*(1-c)+c, y*z*(1-c)-x*s*inv_dist_to_plane,
                             0, 0, 1) * *t;
    t->translate(-d->origin.x(), -d->origin.y());
}

/*!
    \fn void QGraphicsRotation3D::axisChanged()

    This signal is emitted whenever the axis of the object changes.
*/

#include "moc_qgraphicstransform.cpp"

QT_END_NAMESPACE
