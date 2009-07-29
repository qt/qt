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

#include "qmlvaluetype_p.h"

QT_BEGIN_NAMESPACE

QmlValueTypeFactory::QmlValueTypeFactory()
{
    // ### Optimize
    for (int ii = 0; ii < (QVariant::UserType - 1); ++ii)
        valueTypes[ii] = valueType(ii);
}

QmlValueTypeFactory::~QmlValueTypeFactory()
{
    for (int ii = 0; ii < (QVariant::UserType - 1); ++ii)
        delete valueTypes[ii];
}

QmlValueType *QmlValueTypeFactory::valueType(int t)
{
    switch (t) {
    case QVariant::Point:
    case QVariant::PointF:
        return new QmlPointValueType;
    case QVariant::Rect:
    case QVariant::RectF:
        return new QmlRectValueType;
    case QVariant::Vector3D:
        return new QmlVector3DValueType;
    default:
        return 0;
    }
}

QmlValueType::QmlValueType(QObject *parent)
: QObject(parent)
{
}

QmlPointValueType::QmlPointValueType(QObject *parent)
: QmlValueType(parent)
{
}

void QmlPointValueType::read(QObject *obj, int idx)
{
    void *a[] = { &point, 0 };
    QMetaObject::metacall(obj, QMetaObject::ReadProperty, idx, a);
}

void QmlPointValueType::write(QObject *obj, int idx)
{
    void *a[] = { &point, 0 };
    QMetaObject::metacall(obj, QMetaObject::WriteProperty, idx, a);
}

qreal QmlPointValueType::x() const
{
    return point.x();
}

qreal QmlPointValueType::y() const
{
    return point.y();
}

void QmlPointValueType::setX(qreal x)
{
    point.setX(x);
}

void QmlPointValueType::setY(qreal y)
{
    point.setY(y);
}

QmlRectValueType::QmlRectValueType(QObject *parent)
: QmlValueType(parent)
{
}

void QmlRectValueType::read(QObject *obj, int idx)
{
    void *a[] = { &rect, 0 };
    QMetaObject::metacall(obj, QMetaObject::ReadProperty, idx, a);
}

void QmlRectValueType::write(QObject *obj, int idx)
{
    void *a[] = { &rect, 0 };
    QMetaObject::metacall(obj, QMetaObject::WriteProperty, idx, a);
}

qreal QmlRectValueType::x() const
{
    return rect.x();
}

qreal QmlRectValueType::y() const
{
    return rect.y();
}

void QmlRectValueType::setX(qreal x)
{
    rect.moveLeft(x);
}

void QmlRectValueType::setY(qreal y)
{
    rect.moveTop(y);
}

qreal QmlRectValueType::width() const
{
    return rect.width();
}

qreal QmlRectValueType::height() const
{
    return rect.height();
}

void QmlRectValueType::setWidth(qreal w)
{
    rect.setWidth(w);
}

void QmlRectValueType::setHeight(qreal h)
{
    rect.setHeight(h);
}

QmlVector3DValueType::QmlVector3DValueType(QObject *parent)
: QmlValueType(parent)
{
}

void QmlVector3DValueType::read(QObject *obj, int idx)
{
    void *a[] = { &vector, 0 };
    QMetaObject::metacall(obj, QMetaObject::ReadProperty, idx, a);
}

void QmlVector3DValueType::write(QObject *obj, int idx)
{
    void *a[] = { &vector, 0 };
    QMetaObject::metacall(obj, QMetaObject::WriteProperty, idx, a);
}

qreal QmlVector3DValueType::x() const
{
    return vector.x();
}

qreal QmlVector3DValueType::y() const
{
    return vector.y();
}

qreal QmlVector3DValueType::z() const
{
    return vector.z();
}

void QmlVector3DValueType::setX(qreal x)
{
    vector.setX(x);
}

void QmlVector3DValueType::setY(qreal y)
{
    vector.setY(y);
}

void QmlVector3DValueType::setZ(qreal z)
{
    vector.setZ(z);
}

QT_END_NAMESPACE
