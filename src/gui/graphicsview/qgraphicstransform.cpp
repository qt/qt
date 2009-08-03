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

#include "qgraphicstransform.h"
#include "qgraphicsitem_p.h"
#include "qgraphicstransform_p.h"
#include <QDebug>

#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

QT_BEGIN_NAMESPACE


void QGraphicsTransformPrivate::setItem(QGraphicsItem *i)
{
    if (item == i)
        return;

    if (item) {
        Q_Q(QGraphicsTransform);
        QGraphicsItemPrivate *d_ptr = item->d_ptr.data();

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

void QGraphicsTransform::update()
{
    Q_D(QGraphicsTransform);
    if (d->item)
        d->updateItem(d->item);
}

/*!
  returns this object as a QTransform.
*/
QTransform QGraphicsTransform::transform() const
{
    QTransform t;
    applyTo(&t);
    return t;
}

/*!
  \class QGraphicsTransform
  \brief The QGraphicsTransform class is an abstract base class for tranformations on QGraphicsItems.
  \since 4.6

  The classes that inherit QGraphicsTransform express different types of transformations
  that can be applied to graphics items.

  A list of these transformations can be applied to any graphics item. These
  transformations are then easily modifyable and usable from e.g. within animations.

  QGraphicsTransform is an abstract base class that is implemented by QGraphicsScale,
  QGraphicsRotation and QGraphicsRotation3D. Subclasses have to implement the applyTo method.

  \sa QGraphicsItem::transform(), QGraphicsScale, QGraphicsRotation, QGraphicsRotation3D
*/

/*!
  Constructs a new QGraphicsTransform with \a parent.
*/
QGraphicsTransform::QGraphicsTransform(QObject *parent) :
    QObject(*new QGraphicsTransformPrivate, parent)
{
}

/*!
  Destructs the graphics transform.
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

/*! \fn void QGraphicsTransform::applyTo(QTransform *transform) const

  This pure virtual method has to be reimplemented in derived classes.

  It applies this transformation to \a transform.
*/


/*!
  \class QGraphicsScale
  \brief The QGraphicsScale class provides a way to scale a graphics item in 2 dimensions.
  \since 4.6

  QGraphicsScale contains an \a origin around which the scaling happens, and two
  scale factors, xScale and yScale, the x and one for the y axis.
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
  Constructs a new graphics scale object with \a parent.
*/
QGraphicsScale::QGraphicsScale(QObject *parent)
    : QGraphicsTransform(*new QGraphicsScalePrivate, parent)
{
}

/*!
  Destroys the object
*/
QGraphicsScale::~QGraphicsScale()
{
}

/*!
  \property QGraphicsScale::origin
  The origin of the scale. All scaling will be done relative to this point.

  The \a origin is in other words the fixed point for the transformation.
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
  \fn QGraphicsScale::originChanged()

  This signal is emitted whenever the origin of the object
  changes.
*/

/*!
  \property QGraphicsScale::xScale

  The scale factor in x direction. The x direction is
  in the graphics items logical coordinates.

  \sa yScale
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

  The scale factor in y direction. The y direction is
  in the graphics items logical coordinates.

  \sa xScale
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
  \fn QGraphicsScale::scaleChanged()

  This signal is emitted whenever the xScale or yScale of the object
  changes.
*/

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
  \class QGraphicsRotation
  \brief The QGraphicsRotation class provides a way to rotate a graphics item in 2 dimensions.
  \since 4.6

  QGraphicsRotation contains an \a origin around which the rotation happens, and one
  angle in degrees describing the amount of the rotation.
*/

class QGraphicsRotationPrivate : public QGraphicsTransformPrivate
{
public:
    QGraphicsRotationPrivate()
            : angle(0) {}
    QPointF origin;
    qreal originY;
    qreal angle;
};

/*!
  Constructs a new graphics rotation with \a parent.
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
  Destructs the object
*/
QGraphicsRotation::~QGraphicsRotation()
{
}

/*!
  \property QGraphicsRotation::origin
  The origin around which this object will rotate the graphics item.

  The \a origin is in other words the fixed point for the transformation.
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
  \fn QGraphicsRotation::originChanged()

  This signal is emitted whenever the origin of the object
  changes.
*/

/*!
  \property QGraphicsRotation::angle
  The angle, in degrees, of the rotation.
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
  \fn void QGraphicsRotation::angleChanged()

  This signal is emitted whenever the angle of the object
  changes.
*/

/*!
  \reimp
*/
void QGraphicsRotation::applyTo(QTransform *t) const
{
    Q_D(const QGraphicsRotation);
    if(d->angle) {
        t->translate(d->origin.x(), d->origin.y());
        t->rotate(d->angle);
        t->translate(-d->origin.x(), -d->origin.y());
    }
}


/*!
  \class QGraphicsRotation3D
  \brief The QGraphicsRotation3D class provides a way to rotate a graphics item in 3 dimensions.
  \since 4.6

  In addition to the origin and angle of a simple QGraphicsRotation, QGraphicsRotation3D contains
  also an axis that describes around which axis in space the rotation is supposed to happen.
*/

class QGraphicsRotation3DPrivate : public QGraphicsRotationPrivate
{
public:
    QGraphicsRotation3DPrivate() {}

    QVector3D axis;
};

/*!
  Constructs a new 3D rotation with \a parent.
*/
QGraphicsRotation3D::QGraphicsRotation3D(QObject *parent)
    : QGraphicsRotation(*new QGraphicsRotation3DPrivate, parent)
{
}

/*!
  Destroys the object
*/
QGraphicsRotation3D::~QGraphicsRotation3D()
{
}

/*!
    \property QGraphicsRotation3D::axis

    A rotation axis is specified by a vector in 3D space.
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

/*!
  \fn void QGraphicsRotation3D::axisChanged()

  This signal is emitted whenever the axis of the object
  changes.
*/

const qreal inv_dist_to_plane = 1. / 1024.;

/*!
  \reimp
*/
void QGraphicsRotation3D::applyTo(QTransform *t) const
{
    Q_D(const QGraphicsRotation3D);

    if (d->angle == 0. ||
        (d->axis.z() == 0. && d->axis.y() == 0 && d->axis.x() == 0))
        return;

    qreal rad = d->angle * 2. * M_PI / 360.;
    qreal c = ::cos(rad);
    qreal s = ::sin(rad);

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

#include "moc_qgraphicstransform.cpp"

QT_END_NAMESPACE
