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
    case QVariant::Rect:
        return new QmlRectValueType;
    default:
        return 0;
    }
}

QmlValueType::QmlValueType(QObject *parent)
: QObject(parent)
{
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

QT_END_NAMESPACE
