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

#include "qfxflipable.h"
#include "private/qfxitem_p.h"
#include "qfxtransform.h"
#include <QtDeclarative/qmlinfo.h>

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
    if (d->front) {
        qmlInfo(this) << "front is a write-once property";
        return;
    }
    d->front = front;
    children()->append(d->front);
    if (Back == d->current)
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
    if (d->back) {
        qmlInfo(this) << "back is a write-once property";
        return;
    }
    d->back = back;
    children()->append(d->back);
    if (Front == d->current)
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
    if (cross > 0) {
       newSide = Back;
    } else {
        newSide = Front;
    }

    if (newSide != d->current) {
        d->current = newSide;
        if (d->current==Back) {
            QSimpleCanvas::Matrix mat;
#ifdef QFX_RENDER_OPENGL
            mat.translate(d->back->width()/2,d->back->height()/2, 0);
            if (d->back->width() && p1.x() >= p2.x())
                mat.rotate(180, 0, 1, 0);
            if (d->back->height() && p2.y() >= p3.y())
                mat.rotate(180, 1, 0, 0);
            mat.translate(-d->back->width()/2,-d->back->height()/2, 0);
#else
            mat.translate(d->back->width()/2,d->back->height()/2);
            if (d->back->width() && p1.x() >= p2.x())
                mat.rotate(180, Qt::YAxis);
            if (d->back->height() && p2.y() >= p3.y())
                mat.rotate(180, Qt::XAxis);
            mat.translate(-d->back->width()/2,-d->back->height()/2);
#endif
            d->back->setTransform(mat);
        }
        if (d->front)
            d->front->setOpacity((d->current==Front)?1.:0.);
        if (d->back)
            d->back->setOpacity((d->current==Back)?1.:0.);
        emit sideChanged();
    }
}

QT_END_NAMESPACE
