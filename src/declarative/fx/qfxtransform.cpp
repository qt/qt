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

#include <QDebug>
#include "private/qfxitem_p.h"
#include "qfxtransform.h"
#include <QtDeclarative/qmlinfo.h>

#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

QT_BEGIN_NAMESPACE

QML_DEFINE_NOCREATE_TYPE(QFxTransform);

/*!
    \qmlclass Transform
    \brief A transformation.
*/
QFxTransform::QFxTransform(QObject *parent) :
    QObject(parent)
{
}

QFxTransform::~QFxTransform()
{
}

bool QFxTransform::isIdentity() const
{
    return true;
}

QTransform QFxTransform::transform() const
{
    return QTransform();
}

void QFxTransform::update()
{
    QFxItem *item = qobject_cast<QFxItem *>(parent());
    if (item)
        item->updateTransform();
}

/*!
    \qmlclass Scale
    \brief A Scale object provides a way to scale an Item.

    The scale object gives more control over scaling than using Item's scale property. Specifically,
    it allows a different scale for the x and y axes, and allows the scale to be relative to an
    arbitrary point.

    The following example scales the X axis of the Rect, relative to its interior point 25, 25:
    \qml
    Rect {
        width: 100; height: 100
        color: "blue"
        transform: Scale { originX: 25; originY: 25; xScale: 3}
    }
    \endqml
*/

QFxScale::QFxScale(QObject *parent)
: QFxTransform(parent), _originX(0), _originY(0), _xScale(1), _yScale(1), _dirty(true)
{
}

QFxScale::~QFxScale()
{
}

/*!
    \qmlproperty real Scale::originX
    \qmlproperty real Scale::originY

    The origin point for the scale. The scale will be relative to this point.
*/
qreal QFxScale::originX() const
{
    return _originX;
}

void QFxScale::setOriginX(qreal ox)
{
    _originX = ox;
    update();
}

qreal QFxScale::originY() const
{
    return _originY;
}

void QFxScale::setOriginY(qreal oy)
{
    _originY = oy;
    update();
}

/*!
    \qmlproperty real Scale::xScale

    The scaling factor for the X axis.
*/
qreal QFxScale::xScale() const
{
    return _xScale;
}

void QFxScale::setXScale(qreal scale)
{
    if (_xScale == scale)
        return;
    _xScale = scale;
    update();
    emit scaleChanged();
}

/*!
    \qmlproperty real Scale::yScale

    The scaling factor for the Y axis.
*/
qreal QFxScale::yScale() const
{
    return _yScale;
}

void QFxScale::setYScale(qreal scale)
{
    if (_yScale == scale)
        return;
    _yScale = scale;
    update();
    emit scaleChanged();
}

bool QFxScale::isIdentity() const
{
    return (_xScale == 1. && _yScale == 1.);
}

QTransform QFxScale::transform() const
{
    if (_dirty) {
        _transform = QTransform();
        _dirty = false;
        _transform.translate(_originX, _originY);
        _transform.scale(_xScale, _yScale);
        _transform.translate(-_originX, -_originY);
    }
    return _transform;
}

void QFxScale::update()
{
    _dirty = true;
    QFxTransform::update();
}

QML_DEFINE_TYPE(QFxScale, Scale)


/*!
    \qmlclass Axis
    \brief A Axis object defines an axis that can be used for rotation or translation.

    An axis is specified by 2 points in 3D space: a start point and
    an end point. While technically the axis is the line running through these two points
    (and thus many different sets of two points could define the same axis), the distance
    between the points does matter for translation along an axis.

    \image 3d-axis.png

    \qml
    Axis { startX: 20; startY: 0; endX: 40; endY: 60; endZ: 20 }
    \endqml
*/

QML_DEFINE_TYPE(QFxAxis, Axis)

QFxAxis::QFxAxis(QObject *parent)
: QObject(parent), _startX(0), _startY(0), _endX(0), _endY(0), _endZ(0)
{
}

QFxAxis::~QFxAxis()
{
}

/*!
    \qmlproperty real Axis::startX
    \qmlproperty real Axis::startY

    The start point of the axis. The z-position of the start point is assumed to be 0, and cannot
    be changed.
*/
qreal QFxAxis::startX() const
{
    return _startX;
}

void QFxAxis::setStartX(qreal x)
{
    _startX = x;
    emit updated();
}

qreal QFxAxis::startY() const
{
    return _startY;
}

void QFxAxis::setStartY(qreal y)
{
    _startY = y;
    emit updated();
}

/*!
    \qmlproperty real Axis::endX
    \qmlproperty real Axis::endY
    \qmlproperty real Axis::endZ

    The end point of the axis.
*/
qreal QFxAxis::endX() const
{
    return _endX;
}

void QFxAxis::setEndX(qreal x)
{
    _endX = x;
    emit updated();
}

qreal QFxAxis::endY() const
{
    return _endY;
}

void QFxAxis::setEndY(qreal y)
{
    _endY = y;
    emit updated();
}

qreal QFxAxis::endZ() const
{
    return _endZ;
}

void QFxAxis::setEndZ(qreal z)
{
    _endZ = z;
    emit updated();
}

/*!
    \qmlclass Rotation
    \brief A Rotation object provides a way to rotate an Item around a point.

    The following example rotates a Rect around its interior point 25, 25:
    \qml
    Rect {
        width: 100; height: 100
        color: "blue"
        transform: Rotation { originX: 25; originY: 25; angle: 45}
    }
    \endqml
*/

QFxRotation::QFxRotation(QObject *parent)
: QFxTransform(parent), _originX(0), _originY(0), _angle(0), _dirty(true)
{
}

QFxRotation::~QFxRotation()
{
}

/*!
    \qmlproperty real Rotation::originX
    \qmlproperty real Rotation::originY

    The point to rotate around.
*/
qreal QFxRotation::originX() const
{
    return _originX;
}

void QFxRotation::setOriginX(qreal ox)
{
    _originX = ox;
    update();
}

qreal QFxRotation::originY() const
{
    return _originY;
}

void QFxRotation::setOriginY(qreal oy)
{
    _originY = oy;
    update();
}

/*!
    \qmlproperty real Rotation::angle

    The angle, in degrees, to rotate.
*/
qreal QFxRotation::angle() const
{
    return _angle;
}

void QFxRotation::setAngle(qreal angle)
{
    if (_angle == angle)
        return;
    _angle = angle;
    update();
    emit angleChanged();
}

bool QFxRotation::isIdentity() const
{
    return (_angle == 0.);
}

QTransform QFxRotation::transform() const
{
    if (_dirty) {
        _transform = QTransform();
        _dirty = false;
        _transform.translate(_originX, _originY);
        _transform.rotate(_angle);
        _transform.translate(-_originX, -_originY);
    }
    return _transform;
}

void QFxRotation::update()
{
    _dirty = true;
    QFxTransform::update();
}

QML_DEFINE_TYPE(QFxRotation, Rotation)

/*!
    \qmlclass Rotation3D
    \brief A Rotation3D object provides a way to rotate an Item around an axis.

    Here is an example of various rotations applied to an \l Image.
    \snippet doc/src/snippets/declarative/rotation.qml 0

    \image axisrotation.png
*/

QML_DEFINE_TYPE(QFxRotation3D,Rotation3D)

QFxRotation3D::QFxRotation3D(QObject *parent)
: QFxTransform(parent), _angle(0), _dirty(true)
{
    connect(&_axis, SIGNAL(updated()), this, SLOT(update()));
}

QFxRotation3D::~QFxRotation3D()
{
}

/*!
    \qmlproperty real Rotation3D::axis.startX
    \qmlproperty real Rotation3D::axis.startY
    \qmlproperty real Rotation3D::axis.endX
    \qmlproperty real Rotation3D::axis.endY
    \qmlproperty real Rotation3D::axis.endZ

    A rotation axis is specified by 2 points in 3D space: a start point and
    an end point. The z-position of the start point is assumed to be 0, and cannot
    be changed.

    \image 3d-rotation-axis.png

    \sa Axis
*/
QFxAxis *QFxRotation3D::axis()
{
    return &_axis;
}

/*!
    \qmlproperty real Rotation3D::angle

    The angle, in degrees, to rotate around the specified axis.
*/
qreal QFxRotation3D::angle() const
{
    return _angle;
}

void QFxRotation3D::setAngle(qreal angle)
{
    _angle = angle;
    update();
}

bool QFxRotation3D::isIdentity() const
{
    return (_angle == 0.) || (_axis.endZ() == 0. && _axis.endY() == _axis.startY() && _axis.endX() == _axis.startX());
}

const qreal inv_dist_to_plane = 1. / 1024.;
QTransform QFxRotation3D::transform() const
{
    if (_dirty) {
        _transform = QTransform();

        if (!isIdentity()) {
            if (angle() != 0.) {
                QTransform rotTrans;
                rotTrans.translate(-_axis.startX(), -_axis.startY());
                QTransform rotTrans2;
                rotTrans2.translate(_axis.startX(), _axis.startY());

                qreal rad = angle() * 2. * M_PI / 360.;
                qreal c = ::cos(rad);
                qreal s = ::sin(rad);

                qreal x = _axis.endX() - _axis.startX();
                qreal y = _axis.endY() - _axis.startY();
                qreal z = _axis.endZ();

                qreal len = x * x + y * y + z * z;
                if (len != 1.) {
                    len = ::sqrt(len);
                    x /= len;
                    y /= len;
                    z /= len;
                }

                QTransform rot(x*x*(1-c)+c, x*y*(1-c)-z*s, x*z*(1-c)+y*s*inv_dist_to_plane,
                               y*x*(1-c)+z*s, y*y*(1-c)+c, y*z*(1-c)-x*s*inv_dist_to_plane,
                               0, 0, 1);

                _transform *= rotTrans;
                _transform *= rot;
                _transform *= rotTrans2;
            }
        }

        _dirty = false;
    }

    return _transform;
}

void QFxRotation3D::update()
{
    _dirty = true;
    QFxTransform::update();
}

/*!
  \internal
  \qmlclass Translation3D
  \brief A Translation3D object provides a way to move an Item along an axis.

  The following example translates the image to 10, 3.
  \qml
Image {
    source: "logo.png"
    transform: [
        Translation3D {
            axis.startX: 0
            axis.startY: 0
            axis.endX: 1
            axis.endY: .3
            distance: 10
        }
    ]
}
  \endqml
*/

QML_DEFINE_TYPE(QFxTranslation3D,Translation3D)

QFxTranslation3D::QFxTranslation3D(QObject *parent)
: QFxTransform(parent), _distance(0), _dirty(true)
{
    connect(&_axis, SIGNAL(updated()), this, SLOT(update()));
}

QFxTranslation3D::~QFxTranslation3D()
{
}

/*!
  \qmlproperty real Translation3D::axis.startX
  \qmlproperty real Translation3D::axis.startY
  \qmlproperty real Translation3D::axis.endX
  \qmlproperty real Translation3D::axis.endY
  \qmlproperty real Translation3D::axis.endZ

  A translation axis is specified by 2 points in 3D space: a start
  point and an end point. The z-position of the start point is assumed
  to be 0, and cannot be changed. Changing the z-position of the end
  point is only valid when running under OpenGL.

  \sa Axis
*/
QFxAxis *QFxTranslation3D::axis()
{
    return &_axis;
}

/*!
  \qmlproperty real Translation3D::distance

  The distance to translate along the specified axis. distance is a
  multiplier; in the example below, a distance of 1 would translate to
  100, 50, while a distance of 0.5 would translate to 50, 25.

  \qml
  Translation3D { axis.startX: 0; axis.startY: 0; axis.endX: 100; axis.endY: 50 }
  \endqml
*/
qreal QFxTranslation3D::distance() const
{
    return _distance;
}

void QFxTranslation3D::setDistance(qreal distance)
{
    _distance = distance;
    update();
}

bool QFxTranslation3D::isIdentity() const
{
    return (_distance == 0.) || (_axis.endZ() == 0. && _axis.endY() == _axis.startY() && _axis.endX() == _axis.startX());
}

QTransform QFxTranslation3D::transform() const
{
    if (_dirty) {
        _transform = QTransform();

        if (!isIdentity()) {
            if (distance() != 0.) {
                QTransform trans;
                trans.translate((_axis.endX() - _axis.startX()) * distance(),
                        (_axis.endY() - _axis.startY()) * distance());
                _transform *= trans;
            }
        }

        _dirty = false;
    }

    return _transform;
}

void QFxTranslation3D::update()
{
    _dirty = true;

    if (_axis.endZ() != 0. && distance() != 0.) {
        qmlInfo(this) << "QTransform cannot translate along Z-axis.";
    }

    QFxTransform::update();
}

/*!
  \internal
  \qmlclass Perspective
  \brief A Perspective object specifies a perspective transformation.

  A Perspective transform only affects an item when running under
  OpenGL. When running under software rasterization it has no effect.
*/

QML_DEFINE_TYPE(QFxPerspective,Perspective)

QFxPerspective::QFxPerspective(QObject *parent)
    : QFxTransform(parent)
{
}

QFxPerspective::~QFxPerspective()
{
}

/*!
    \qmlproperty real Perspective::angle
*/

/*!
    \qmlproperty real Perspective::aspect
*/

/*!
    \qmlproperty real Perspective::x
*/

/*!
    \qmlproperty real Perspective::y
*/

/*!
    \qmlproperty real Perspective::scale
*/

/*!
    \qmlclass Squish
    \brief A Squish object allows you to distort an item's appearance by 'squishing' it.

    Conceptually, a Squish works by allowing you to move the four corners of an item,
    and distorting the item to fit into the newly created polygon.

    \image squish-transform.png

    Here is an example of various \l Image squishes.
    \qml
    Rect {
        id: Screen
        width: 360; height: 80
        color: "white"

        HorizontalLayout {
            margin: 10
            spacing: 10
            Image { source: "qt.png" }
            Image {
                source: "qt.png"
                transform: Squish {
                    x:0; y:0; width:60; height:60
                    topLeftX:0; topLeftY:0
                    topRightX:50; topRightY:10
                    bottomLeftX:0; bottomLeftY:60
                    bottomRightX: 60; bottomRightY:60
                }
            }
            Image {
                source: "qt.png"
                transform: Squish {
                    x:0; y:0; width:60; height:60
                    topLeftX:0; topLeftY:0
                    topRightX:50; topRightY:0
                    bottomLeftX:10; bottomLeftY:50
                    bottomRightX: 60; bottomRightY:60
                }
            }
            Image {
                source: "qt.png"
                transform: Squish {
                    x:0; y:0; width:60; height:60
                    topLeftX:0; topLeftY:10
                    topRightX:60; topRightY:10
                    bottomLeftX:0; bottomLeftY:50
                    bottomRightX: 60; bottomRightY:50
                }
            }
            Image {
                source: "qt.png"
                transform: Squish {
                    x:0; y:0; width:60; height:60
                    topLeftX:10; topLeftY:0
                    topRightX:50; topRightY:0
                    bottomLeftX:10; bottomLeftY:60
                    bottomRightX: 50; bottomRightY:60
                }
            }
        }
    }
    \endqml

    \image squish.png
*/
QML_DEFINE_TYPE(QFxSquish,Squish)

QFxSquish::QFxSquish(QObject *parent)
    : QFxTransform(parent)
{
}

QFxSquish::~QFxSquish()
{
}

/*!
    \qmlproperty real Squish::x
    \qmlproperty real Squish::y
    \qmlproperty real Squish::width
    \qmlproperty real Squish::height

    This is usually set to the original geometry of the item being squished.
*/
qreal QFxSquish::x() const
{
    return p.x();
}

void QFxSquish::setX(qreal v)
{
    p.setX(v);
    update();
}

qreal QFxSquish::y() const
{
    return p.y();
}

void QFxSquish::setY(qreal v)
{
    p.setY(v);
    update();
}

qreal QFxSquish::width() const
{
    return s.width();
}

void QFxSquish::setWidth(qreal v)
{
    s.setWidth(v);
    update();
}

qreal QFxSquish::height() const
{
    return s.height();
}

void QFxSquish::setHeight(qreal v)
{
    s.setHeight(v);
    update();
}

/*!
    \qmlproperty real Squish::topLeftX
    \qmlproperty real Squish::topLeftY

    The top left point for the squish.
*/
qreal QFxSquish::topLeft_x() const
{
    return p1.x();
}

void QFxSquish::settopLeft_x(qreal v)
{
    p1.setX(v);
    update();
}

qreal QFxSquish::topLeft_y() const
{
    return p1.y();
}

void QFxSquish::settopLeft_y(qreal v)
{
    p1.setY(v);
    update();
}

/*!
    \qmlproperty real Squish::topRightX
    \qmlproperty real Squish::topRightY

    The top right point for the squish.
*/
qreal QFxSquish::topRight_x() const
{
    return p2.x();
}

void QFxSquish::settopRight_x(qreal v)
{
    p2.setX(v);
    update();
}

qreal QFxSquish::topRight_y() const
{
    return p2.y();
}

void QFxSquish::settopRight_y(qreal v)
{
    p2.setY(v);
    update();
}

/*!
    \qmlproperty real Squish::bottomLeftX
    \qmlproperty real Squish::bottomLeftY

    The bottom left point for the squish.
*/
qreal QFxSquish::bottomLeft_x() const
{
    return p3.x();
}

void QFxSquish::setbottomLeft_x(qreal v)
{
    p3.setX(v);
    update();
}

qreal QFxSquish::bottomLeft_y() const
{
    return p3.y();
}

void QFxSquish::setbottomLeft_y(qreal v)
{
    p3.setY(v);
    update();
}

/*!
    \qmlproperty real Squish::bottomRightX
    \qmlproperty real Squish::bottomRightY

    The bottom right point for the squish.
*/
qreal QFxSquish::bottomRight_x() const
{
    return p4.x();
}

void QFxSquish::setbottomRight_x(qreal v)
{
    p4.setX(v);
    update();
}

qreal QFxSquish::bottomRight_y() const
{
    return p4.y();
}

void QFxSquish::setbottomRight_y(qreal v)
{
    p4.setY(v);
    update();
}

bool QFxSquish::isIdentity() const
{
    return false;
}

QTransform QFxSquish::transform() const
{
    QPolygonF poly;
    poly << p << QPointF(p.x() + s.width(), p.y()) << QPointF(p.x() + s.width(), p.y() + s.height()) << QPointF(p.x(), p.y() + s.height());
    QPolygonF poly2;
    poly2 << p1 << p2 << p4 << p3;

    QTransform t;
    QTransform::quadToQuad(poly, poly2, t);
    return t;
}

QT_END_NAMESPACE
