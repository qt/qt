/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "private/qdeclarativetranslate_p.h"
#include <private/qgraphicstransform_p.h>
#include <QDebug>
#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

class QDeclarativeTranslatePrivate : public QGraphicsTransformPrivate
{
public:
    QDeclarativeTranslatePrivate()
        : x(0), y(0) {}
    qreal x;
    qreal y;
};

/*!
    Constructs an empty QDeclarativeTranslate object with the given \a parent.
*/
QDeclarativeTranslate::QDeclarativeTranslate(QObject *parent)
    : QGraphicsTransform(*new QDeclarativeTranslatePrivate, parent)
{
}

/*!
    Destroys the graphics scale.
*/
QDeclarativeTranslate::~QDeclarativeTranslate()
{
}

/*!
    \property QDeclarativeTranslate::x
    \brief the horizontal translation.

    The translation can be any real number; the default value is 0.0.

    \sa y
*/
qreal QDeclarativeTranslate::x() const
{
    Q_D(const QDeclarativeTranslate);
    return d->x;
}
void QDeclarativeTranslate::setX(qreal x)
{
    Q_D(QDeclarativeTranslate);
    if (d->x == x)
        return;
    d->x = x;
    update();
    emit positionChanged();
}

/*!
    \property QDeclarativeTranslate::y
    \brief the vertical translation.

    The translation can be any real number; the default value is 0.0.

    \sa x
*/
qreal QDeclarativeTranslate::y() const
{
    Q_D(const QDeclarativeTranslate);
    return d->y;
}
void QDeclarativeTranslate::setY(qreal y)
{
    Q_D(QDeclarativeTranslate);
    if (d->y == y)
        return;
    d->y = y;
    update();
    emit positionChanged();
}

/*!
    \internal
*/
void QDeclarativeTranslate::applyTo(QMatrix4x4 *matrix) const
{
    Q_D(const QDeclarativeTranslate);
    matrix->translate(d->x, d->y, 0);
}

/*!
    \fn QDeclarativeTranslate::positionChanged()

    QDeclarativeTranslate emits this signal when its position changes.

    \sa QDeclarativeTranslate::x, QDeclarativeTranslate::y
*/

QT_END_NAMESPACE
