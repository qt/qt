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

QT_BEGIN_NAMESPACE
#define M_PI 3.14159265358979323846
#endif

QML_DEFINE_TYPE(QFxTransform,Transform);

/*!
    \qmlclass Transform
    \brief A transformation.

    \todo Document Transform.
*/
QFxTransform::QFxTransform(QObject *parent) :
    QObject(parent)
{
}

bool QFxTransform::isIdentity() const
{
    return true;
}

QSimpleCanvas::Matrix QFxTransform::transform() const
{
    return QSimpleCanvas::Matrix();
}

void QFxTransform::update()
{
    QFxItem *item = qobject_cast<QFxItem *>(parent());
    if(item)
        item->updateTransform();
}

QML_DEFINE_TYPE(QFxAxis,Axis);

QFxAxis::QFxAxis(QObject *parent)
: QFxTransform(parent), _xStart(0), _yStart(0), _xEnd(0), _yEnd(0), _zEnd(0), _rotation(0), 
  _translation(0), _distanceToPlane(1024.), _dirty(true)
{
}

qreal QFxAxis::xStart() const
{
    return _xStart;
}

void QFxAxis::setXStart(qreal x)
{
    _xStart = x;
    update();
}

qreal QFxAxis::yStart() const
{
    return _yStart;
}

void QFxAxis::setYStart(qreal y)
{
    _yStart = y;
    update();
}

qreal QFxAxis::xEnd() const
{
    return _xEnd;
}

void QFxAxis::setXEnd(qreal x)
{
    _xEnd = x;
    update();
}

qreal QFxAxis::yEnd() const
{
    return _yEnd;
}

void QFxAxis::setYEnd(qreal y)
{
    _yEnd = y;
    update();
}

qreal QFxAxis::zEnd() const
{
    return _zEnd;
}

void QFxAxis::setZEnd(qreal z)
{
#if !defined(QFX_RENDER_OPENGL)
    if(z != 0. && translation() != 0.) {
        qmlInfo(this) << "QTransform cannot translate along Z-axis";
        return;
    }
#endif

    _zEnd = z;
    update();
}

qreal QFxAxis::rotation() const
{
    return _rotation;
}

void QFxAxis::setRotation(qreal r)
{
    _rotation = r;
    update();
}

qreal QFxAxis::translation() const
{
    return _translation;
}

void QFxAxis::setTranslation(qreal t)
{
#if !defined(QFX_RENDER_OPENGL)
    if(zEnd() != 0. && t != 0.) {
        qmlInfo(this) << "QTransform cannot translate along Z-axis";
        return;
    }
#endif

    _translation = t;
    update();
}

bool QFxAxis::isIdentity() const
{
    return (_rotation == 0. && _translation == 0.) || 
           (zEnd() == 0. && yEnd() == yStart() && xEnd() == xStart());
}

#if defined(QFX_RENDER_QPAINTER)
const qreal inv_dist_to_plane = 1. / 1024.;
QTransform QFxAxis::transform() const
{
    if(_dirty) {
        _transform = QTransform();

        if(!isIdentity()) {
            if(rotation() != 0.) {
                QTransform rotTrans;
                rotTrans.translate(-xStart(), -yStart());
                QTransform rotTrans2;
                rotTrans2.translate(xStart(), yStart());

                qreal rad = rotation() * 2. * M_PI / 360.;
                qreal c = ::cos(rad);
                qreal s = ::sin(rad);

                qreal x = xEnd() - xStart();
                qreal y = yEnd() - yStart();
                qreal z = zEnd();

                qreal idtp = inv_dist_to_plane;
                if(distanceToPlane() != 1024.)
                    idtp = 1. / distanceToPlane();

                qreal len = x * x + y * y + z * z;
                if(len != 1.) {
                    len = ::sqrt(len);
                    x /= len;
                    y /= len;
                    z /= len;
                }

                QTransform rot(x*x*(1-c)+c, x*y*(1-c)-z*s, x*z*(1-c)+y*s*idtp,
                               y*x*(1-c)+z*s, y*y*(1-c)+c, y*z*(1-c)-x*s*idtp,
                               0, 0, 1);

                _transform *= rotTrans;
                _transform *= rot;
                _transform *= rotTrans2;
            }

            if(translation() != 0.) {
                QTransform trans;
                trans.translate((xEnd() - xStart()) * translation(),
                        (yEnd() - yStart()) * translation());
                _transform *= trans;
            }
        }

        _dirty = false;
    }

    return _transform;
}
#elif defined(QFX_RENDER_OPENGL)
QMatrix4x4 QFxAxis::transform() const
{
    if(_dirty) {
        _dirty = false;
        _transform = QMatrix4x4();

        if(!isIdentity()) {
            if(rotation() != 0.) {
                qreal x = xEnd() - xStart();
                qreal y = yEnd() - yStart();
                qreal z = zEnd();

                _transform.translate(xStart(), yStart(), 0);
                _transform.rotate(rotation(), x, y, z);
                _transform.translate(-xStart(), -yStart(), 0);
            }

            if(translation() != 0.) 
                _transform.translate((xEnd() - xStart()) * translation(),
                                     (yEnd() - yStart()) * translation(),
                                     (zEnd()) * translation());
        }
    }

    return _transform;
}
#endif

qreal QFxAxis::distanceToPlane() const
{
    return _distanceToPlane;
}

void QFxAxis::setDistanceToPlane(qreal d)
{
    _distanceToPlane = d;
    update();
}

void QFxAxis::update()
{
    _dirty = true;
    QFxItem *item = qobject_cast<QFxItem *>(parent());
    if(item)
        item->updateTransform();
}

QML_DEFINE_TYPE(QFxFlipable,Flipable);

class QFxFlipablePrivate : public QFxItemPrivate
{
public:
    QFxFlipablePrivate() : current(QFxFlipable::Front), front(0), back(0) {}

    QFxFlipable::Side current;
    QFxItem *front;
    QFxItem *back;
};

/*!
    \qmlclass Flipable QFxFlipable
    \brief The Flipable element provides a surface that can be flipped.
    \inherits Item

    Flipable allows you to specify a front and a back and then flip between those sides.

    \code
    <Flipable id="flipable" width="40" height="40">
        <transform>
            <Axis id="axis" xStart="20" xEnd="20" yStart="20" yEnd="0" />
        </transform>
        <front>
            <Image file="front.png"/>
        </front>
        <back>
            <Image file="back.png"/>
        </back>
        <states>
            <State name="back">
                <SetProperty target="{axis}" property="rotation" value="180" />
            </State>
        </states>
        <transitions>
            <Transition>
                <NumericAnimation easing="easeInOutQuad" properties="rotation"/>
            </Transition>
        </transitions>
    </Flipable>
    \endcode

    \image flipable.gif

    \todo A lot needs to be done to get a fully-functioning Flipable. Should we simplify?

*/

/*!
    \internal
    \class QFxFlipable
    \brief The QFxFlipable class provides a flipable surface.

    \ingroup group_widgets

    QFxFlipable allows you to specify a front and a back, as well as an
    axis for the flip.
*/

QFxFlipable::QFxFlipable(QFxItem *parent)
: QFxItem(*(new QFxFlipablePrivate), parent)
{
}

QFxFlipable::~QFxFlipable()
{
}

/*!
  \qmlproperty Item Flipable::front
  \qmlproperty Item Flipable::back

  The front and back sides of the flipable.
*/

QFxItem *QFxFlipable::front()
{
    Q_D(const QFxFlipable);
    return d->front;
}

void QFxFlipable::setFront(QFxItem *front)
{
    Q_D(QFxFlipable);
    if(d->front) {
        qmlInfo(this) << "front is a write-once property";
        return;
    }
    d->front = front;
    children()->append(d->front);
    if(Back == d->current)
        d->front->setOpacity(0.);
}

QFxItem *QFxFlipable::back()
{
    Q_D(const QFxFlipable);
    return d->back;
}

void QFxFlipable::setBack(QFxItem *back)
{
    Q_D(QFxFlipable);
    if(d->back) {
        qmlInfo(this) << "back is a write-once property";
        return;
    }
    d->back = back;
    children()->append(d->back);
    if(Front == d->current)
        d->back->setOpacity(0.);
}

/*!
  \qmlproperty enumeration Flipable::side

  The side of the Flippable currently visible. Possible values are \c
  Front and \c Back.
*/
QFxFlipable::Side QFxFlipable::side() const
{
    Q_D(const QFxFlipable);
    return d->current;
}

void QFxFlipable::transformChanged(const QSimpleCanvas::Matrix &trans)
{
    Q_D(QFxFlipable);
    QPointF p1(0, 0);
    QPointF p2(1, 0);
    QPointF p3(1, 1);

    p1 = trans.map(p1);
    p2 = trans.map(p2);
    p3 = trans.map(p3);

    qreal cross = (p1.x() - p2.x()) * (p3.y() - p2.y()) -
                  (p1.y() - p2.y()) * (p3.x() - p2.x());

    Side newSide;
    if(cross > 0) {
       newSide = Back;
    } else {
        newSide = Front;
    }

    if(newSide != d->current) {
        d->current = newSide;
        if (d->current==Back) {
            QSimpleCanvas::Matrix mat;
#ifdef QFX_RENDER_OPENGL
            mat.translate(d->back->width()/2,d->back->height()/2, 0);
            if(d->back->width() && p1.x() >= p2.x())
                mat.rotate(180, 0, 1, 0);
            if(d->back->height() && p2.y() >= p3.y())
                mat.rotate(180, 1, 0, 0);
            mat.translate(-d->back->width()/2,-d->back->height()/2, 0);
#else
            mat.translate(d->back->width()/2,d->back->height()/2);
            if(d->back->width() && p1.x() >= p2.x())
                mat.rotate(180, Qt::YAxis);
            if(d->back->height() && p2.y() >= p3.y())
                mat.rotate(180, Qt::XAxis);
            mat.translate(-d->back->width()/2,-d->back->height()/2);
#endif
            d->back->setTransform(mat);
        }
        if(d->front)
            d->front->setOpacity((d->current==Front)?1.:0.);
        if(d->back)
            d->back->setOpacity((d->current==Back)?1.:0.);
        emit sideChanged();
    }
}

QML_DEFINE_TYPE(QFxPerspective,Perspective);

QFxPerspective::QFxPerspective(QObject *parent)
    : QFxTransform(parent)
{
}

#if defined(QFX_RENDER_OPENGL)
bool QFxPerspective::isIdentity() const
{
    return false;
}

QMatrix4x4 QFxPerspective::transform() const
{
    QMatrix4x4 rv;
    rv.translate(_x, _y);
    rv.perspective(_angle, _aspect, 1, 1024 * 1024);
    rv.translate(-_x, -_y, -1);
    rv.scale(1, 1, 1. / _scale);

    return rv;
}
#endif

QML_DEFINE_TYPE(QFxSquish,Squish);

QFxSquish::QFxSquish(QObject *parent)
    : QFxTransform(parent)
{
}

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

void QFxSquish::update()
{
    QFxItem *item = qobject_cast<QFxItem *>(parent());
    if(item)
        item->updateTransform();
}

#if defined(QFX_RENDER_OPENGL)
bool QFxSquish::isIdentity() const
{
    return false;
}

QMatrix4x4 QFxSquish::transform() const
{
    QPolygonF poly;
    poly << p << QPointF(p.x() + s.width(), p.y()) << QPointF(p.x() + s.width(), p.y() + s.height()) << QPointF(p.x(), p.y() + s.height());
    QPolygonF poly2;
    poly2 << p1 << p2 << p4 << p3;

    QTransform t;
    QMatrix4x4 rv;
    if(QTransform::quadToQuad(poly, poly2, t))
        rv = QMatrix4x4(t);

    return rv;
}
QT_END_NAMESPACE
#endif
