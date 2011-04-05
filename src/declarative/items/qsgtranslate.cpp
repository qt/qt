// Commit: ac5c099cc3c5b8c7eec7a49fdeb8a21037230350
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

#include "qsgtranslate_p.h"
#include "qsgitem_p.h"

#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

class QSGTranslatePrivate : public QSGTransformPrivate
{
public:
    QSGTranslatePrivate()
    : x(0), y(0) {}

    qreal x;
    qreal y;
};

QSGTranslate::QSGTranslate(QObject *parent)
: QSGTransform(*new QSGTranslatePrivate, parent)
{
}

QSGTranslate::~QSGTranslate()
{
}

qreal QSGTranslate::x() const
{
    Q_D(const QSGTranslate);
    return d->x;
}

void QSGTranslate::setX(qreal x)
{
    Q_D(QSGTranslate);
    if (d->x == x)
        return;
    d->x = x;
    update();
    emit xChanged();
}

qreal QSGTranslate::y() const
{
    Q_D(const QSGTranslate);
    return d->y;
}
void QSGTranslate::setY(qreal y)
{
    Q_D(QSGTranslate);
    if (d->y == y)
        return;
    d->y = y;
    update();
    emit yChanged();
}

void QSGTranslate::applyTo(QMatrix4x4 *matrix) const
{
    Q_D(const QSGTranslate);
    matrix->translate(d->x, d->y, 0);
}

class QSGScalePrivate : public QSGTransformPrivate
{
public:
    QSGScalePrivate()
        : xScale(1), yScale(1), zScale(1) {}
    QVector3D origin;
    qreal xScale;
    qreal yScale;
    qreal zScale;
};

QSGScale::QSGScale(QObject *parent)
    : QSGTransform(*new QSGScalePrivate, parent)
{
}

QSGScale::~QSGScale()
{
}

QVector3D QSGScale::origin() const
{
    Q_D(const QSGScale);
    return d->origin;
}
void QSGScale::setOrigin(const QVector3D &point)
{
    Q_D(QSGScale);
    if (d->origin == point)
        return;
    d->origin = point;
    update();
    emit originChanged();
}

qreal QSGScale::xScale() const
{
    Q_D(const QSGScale);
    return d->xScale;
}
void QSGScale::setXScale(qreal scale)
{
    Q_D(QSGScale);
    if (d->xScale == scale)
        return;
    d->xScale = scale;
    update();
    emit xScaleChanged();
    emit scaleChanged();
}

qreal QSGScale::yScale() const
{
    Q_D(const QSGScale);
    return d->yScale;
}
void QSGScale::setYScale(qreal scale)
{
    Q_D(QSGScale);
    if (d->yScale == scale)
        return;
    d->yScale = scale;
    update();
    emit yScaleChanged();
    emit scaleChanged();
}

qreal QSGScale::zScale() const
{
    Q_D(const QSGScale);
    return d->zScale;
}
void QSGScale::setZScale(qreal scale)
{
    Q_D(QSGScale);
    if (d->zScale == scale)
        return;
    d->zScale = scale;
    update();
    emit zScaleChanged();
    emit scaleChanged();
}

void QSGScale::applyTo(QMatrix4x4 *matrix) const
{
    Q_D(const QSGScale);
    matrix->translate(d->origin);
    matrix->scale(d->xScale, d->yScale, d->zScale);
    matrix->translate(-d->origin);
}

class QSGRotationPrivate : public QSGTransformPrivate
{
public:
    QSGRotationPrivate()
        : angle(0), axis(0, 0, 1) {}
    QVector3D origin;
    qreal angle;
    QVector3D axis;
};

QSGRotation::QSGRotation(QObject *parent)
    : QSGTransform(*new QSGRotationPrivate, parent)
{
}

QSGRotation::~QSGRotation()
{
}

QVector3D QSGRotation::origin() const
{
    Q_D(const QSGRotation);
    return d->origin;
}

void QSGRotation::setOrigin(const QVector3D &point)
{
    Q_D(QSGRotation);
    if (d->origin == point)
        return;
    d->origin = point;
    update();
    emit originChanged();
}

qreal QSGRotation::angle() const
{
    Q_D(const QSGRotation);
    return d->angle;
}
void QSGRotation::setAngle(qreal angle)
{
    Q_D(QSGRotation);
    if (d->angle == angle)
        return;
    d->angle = angle;
    update();
    emit angleChanged();
}

QVector3D QSGRotation::axis() const
{
    Q_D(const QSGRotation);
    return d->axis;
}
void QSGRotation::setAxis(const QVector3D &axis)
{
    Q_D(QSGRotation);
    if (d->axis == axis)
         return;
    d->axis = axis;
    update();
    emit axisChanged();
}

void QSGRotation::setAxis(Qt::Axis axis)
{
    switch (axis)
    {
    case Qt::XAxis:
        setAxis(QVector3D(1, 0, 0));
        break;
    case Qt::YAxis:
        setAxis(QVector3D(0, 1, 0));
        break;
    case Qt::ZAxis:
        setAxis(QVector3D(0, 0, 1));
        break;
    }
}

struct QGraphicsRotation {
    static inline void projectedRotate(QMatrix4x4 *matrix, qreal angle, qreal x, qreal y, qreal z)
    {
        matrix->projectedRotate(angle, x, y, z);
    }
};

void QSGRotation::applyTo(QMatrix4x4 *matrix) const
{
    Q_D(const QSGRotation);

    if (d->angle == 0. || d->axis.isNull())
        return;

    matrix->translate(d->origin);
    QGraphicsRotation::projectedRotate(matrix, d->angle, d->axis.x(), d->axis.y(), d->axis.z());
    matrix->translate(-d->origin);
}

QT_END_NAMESPACE
