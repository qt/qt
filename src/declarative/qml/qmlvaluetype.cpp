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
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

QmlValueTypeFactory::QmlValueTypeFactory()
{
    // ### Optimize
    for (unsigned int ii = 0; ii < (QVariant::UserType - 1); ++ii)
        valueTypes[ii] = valueType(ii);
}

QmlValueTypeFactory::~QmlValueTypeFactory()
{
    for (unsigned int ii = 0; ii < (QVariant::UserType - 1); ++ii)
        delete valueTypes[ii];
}

QmlValueType *QmlValueTypeFactory::valueType(int t)
{
    switch (t) {
    case QVariant::Point:
        return new QmlPointValueType;
    case QVariant::PointF:
        return new QmlPointFValueType;
    case QVariant::Size:
        return new QmlSizeValueType;
    case QVariant::SizeF:
        return new QmlSizeFValueType;
    case QVariant::Rect:
        return new QmlRectValueType;
    case QVariant::RectF:
        return new QmlRectFValueType;
    case QVariant::Vector3D:
        return new QmlVector3DValueType;
    case QVariant::Font:
        return new QmlFontValueType;
    default:
        return 0;
    }
}

QmlValueType::QmlValueType(QObject *parent)
: QObject(parent)
{
}

QmlPointFValueType::QmlPointFValueType(QObject *parent)
: QmlValueType(parent)
{
}

void QmlPointFValueType::read(QObject *obj, int idx)
{
    void *a[] = { &point, 0 };
    QMetaObject::metacall(obj, QMetaObject::ReadProperty, idx, a);
}

void QmlPointFValueType::write(QObject *obj, int idx)
{
    void *a[] = { &point, 0 };
    QMetaObject::metacall(obj, QMetaObject::WriteProperty, idx, a);
}

qreal QmlPointFValueType::x() const
{
    return point.x();
}

qreal QmlPointFValueType::y() const
{
    return point.y();
}

void QmlPointFValueType::setX(qreal x)
{
    point.setX(x);
}

void QmlPointFValueType::setY(qreal y)
{
    point.setY(y);
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

int QmlPointValueType::x() const
{
    return point.x();
}

int QmlPointValueType::y() const
{
    return point.y();
}

void QmlPointValueType::setX(int x)
{
    point.setX(x);
}

void QmlPointValueType::setY(int y)
{
    point.setY(y);
}

QmlSizeFValueType::QmlSizeFValueType(QObject *parent)
: QmlValueType(parent)
{
}

void QmlSizeFValueType::read(QObject *obj, int idx)
{
    void *a[] = { &size, 0 };
    QMetaObject::metacall(obj, QMetaObject::ReadProperty, idx, a);
}

void QmlSizeFValueType::write(QObject *obj, int idx)
{
    void *a[] = { &size, 0 };
    QMetaObject::metacall(obj, QMetaObject::WriteProperty, idx, a);
}

qreal QmlSizeFValueType::width() const
{
    return size.width();
}

qreal QmlSizeFValueType::height() const
{
    return size.height();
}

void QmlSizeFValueType::setWidth(qreal w)
{
    size.setWidth(w);
}

void QmlSizeFValueType::setHeight(qreal h)
{
    size.setHeight(h);
}

QmlSizeValueType::QmlSizeValueType(QObject *parent)
: QmlValueType(parent)
{
}

void QmlSizeValueType::read(QObject *obj, int idx)
{
    void *a[] = { &size, 0 };
    QMetaObject::metacall(obj, QMetaObject::ReadProperty, idx, a);
}

void QmlSizeValueType::write(QObject *obj, int idx)
{
    void *a[] = { &size, 0 };
    QMetaObject::metacall(obj, QMetaObject::WriteProperty, idx, a);
}

int QmlSizeValueType::width() const
{
    return size.width();
}

int QmlSizeValueType::height() const
{
    return size.height();
}

void QmlSizeValueType::setWidth(int w)
{
    size.setWidth(w);
}

void QmlSizeValueType::setHeight(int h)
{
    size.setHeight(h);
}

QmlRectFValueType::QmlRectFValueType(QObject *parent)
: QmlValueType(parent)
{
}

void QmlRectFValueType::read(QObject *obj, int idx)
{
    void *a[] = { &rect, 0 };
    QMetaObject::metacall(obj, QMetaObject::ReadProperty, idx, a);
}

void QmlRectFValueType::write(QObject *obj, int idx)
{
    void *a[] = { &rect, 0 };
    QMetaObject::metacall(obj, QMetaObject::WriteProperty, idx, a);
}

qreal QmlRectFValueType::x() const
{
    return rect.x();
}

qreal QmlRectFValueType::y() const
{
    return rect.y();
}

void QmlRectFValueType::setX(qreal x)
{
    rect.moveLeft(x);
}

void QmlRectFValueType::setY(qreal y)
{
    rect.moveTop(y);
}

qreal QmlRectFValueType::width() const
{
    return rect.width();
}

qreal QmlRectFValueType::height() const
{
    return rect.height();
}

void QmlRectFValueType::setWidth(qreal w)
{
    rect.setWidth(w);
}

void QmlRectFValueType::setHeight(qreal h)
{
    rect.setHeight(h);
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

int QmlRectValueType::x() const
{
    return rect.x();
}

int QmlRectValueType::y() const
{
    return rect.y();
}

void QmlRectValueType::setX(int x)
{
    rect.moveLeft(x);
}

void QmlRectValueType::setY(int y)
{
    rect.moveTop(y);
}

int QmlRectValueType::width() const
{
    return rect.width();
}

int QmlRectValueType::height() const
{
    return rect.height();
}

void QmlRectValueType::setWidth(int w)
{
    rect.setWidth(w);
}

void QmlRectValueType::setHeight(int h)
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

QmlFontValueType::QmlFontValueType(QObject *parent)
: QmlValueType(parent), hasPixelSize(false)
{
}

void QmlFontValueType::read(QObject *obj, int idx)
{
    void *a[] = { &font, 0 };
    QMetaObject::metacall(obj, QMetaObject::ReadProperty, idx, a);
}

void QmlFontValueType::write(QObject *obj, int idx)
{
    void *a[] = { &font, 0 };
    QMetaObject::metacall(obj, QMetaObject::WriteProperty, idx, a);
}

QString QmlFontValueType::family() const
{
    return font.family();
}

void QmlFontValueType::setFamily(const QString &family)
{
    font.setFamily(family);
}

bool QmlFontValueType::bold() const
{
    return font.bold();
}

void QmlFontValueType::setBold(bool b)
{
    font.setBold(b);
}

bool QmlFontValueType::italic() const
{
    return font.italic();
}

void QmlFontValueType::setItalic(bool b)
{
    font.setItalic(b);
}

bool QmlFontValueType::underline() const
{
    return font.underline();
}

void QmlFontValueType::setUnderline(bool b)
{
    font.setUnderline(b);
}

qreal QmlFontValueType::pointSize() const
{
    return font.pointSizeF();
}

void QmlFontValueType::setPointSize(qreal size)
{
    if (hasPixelSize) {
        qWarning() << "Both point size and pixel size set. Using pixel size.";
        return;
    }
    font.setPointSizeF(size);
}

int QmlFontValueType::pixelSize() const
{
    return font.pixelSize();
}

void QmlFontValueType::setPixelSize(int size)
{
    font.setPixelSize(size);
    hasPixelSize = true;
}

QT_END_NAMESPACE
