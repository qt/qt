// Commit: caee66da925949cf7aef2ff8e1a86c38dd6e6efd
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

#include "qsgflipable_p.h"
#include "qsgitem_p.h"

#include <private/qdeclarativeguard_p.h>

#include <QtDeclarative/qdeclarativeinfo.h>

QT_BEGIN_NAMESPACE

// XXX todo - i think this needs work and a bit of a re-think

class QSGLocalTransform : public QSGTransform
{
    Q_OBJECT
public:
    QSGLocalTransform(QObject *parent) : QSGTransform(parent) {}

    void setTransform(const QTransform &t) {
        transform = t;
        update();
    }
    virtual void applyTo(QMatrix4x4 *matrix) const {
        *matrix *= transform;
    }
private:
    QTransform transform;
};

class QSGFlipablePrivate : public QSGItemPrivate
{
    Q_DECLARE_PUBLIC(QSGFlipable)
public:
    QSGFlipablePrivate() : current(QSGFlipable::Front), front(0), back(0), sideDirty(false) {}

    virtual void transformChanged();
    void updateSide();
    void setBackTransform();

    QSGFlipable::Side current;
    QDeclarativeGuard<QSGLocalTransform> backTransform;
    QDeclarativeGuard<QSGItem> front;
    QDeclarativeGuard<QSGItem> back;

    bool sideDirty;
    bool wantBackXFlipped;
    bool wantBackYFlipped;
};

QSGFlipable::QSGFlipable(QSGItem *parent)
: QSGItem(*(new QSGFlipablePrivate), parent)
{
}

QSGFlipable::~QSGFlipable()
{
}

QSGItem *QSGFlipable::front()
{
    Q_D(const QSGFlipable);
    return d->front;
}

void QSGFlipable::setFront(QSGItem *front)
{
    Q_D(QSGFlipable);
    if (d->front) {
        qmlInfo(this) << tr("front is a write-once property");
        return;
    }
    d->front = front;
    d->front->setParentItem(this);
    if (Back == d->current)
        d->front->setOpacity(0.);
    emit frontChanged();
}

QSGItem *QSGFlipable::back()
{
    Q_D(const QSGFlipable);
    return d->back;
}

void QSGFlipable::setBack(QSGItem *back)
{
    Q_D(QSGFlipable);
    if (d->back) {
        qmlInfo(this) << tr("back is a write-once property");
        return;
    }
    if (back == 0)
        return;
    d->back = back;
    d->back->setParentItem(this);

    d->backTransform = new QSGLocalTransform(d->back);
    d->backTransform->prependToItem(d->back);

    if (Front == d->current)
        d->back->setOpacity(0.);
    connect(back, SIGNAL(widthChanged()),
            this, SLOT(retransformBack()));
    connect(back, SIGNAL(heightChanged()),
            this, SLOT(retransformBack()));
    emit backChanged();
}

void QSGFlipable::retransformBack()
{
    Q_D(QSGFlipable);
    if (d->current == QSGFlipable::Back && d->back)
        d->setBackTransform();
}

QSGFlipable::Side QSGFlipable::side() const
{
    Q_D(const QSGFlipable);

    const_cast<QSGFlipablePrivate *>(d)->updateSide();
    return d->current;
}

void QSGFlipablePrivate::transformChanged()
{
    Q_Q(QSGFlipable);

    if (!sideDirty) {
        sideDirty = true;
        q->polish();
    }

    QSGItemPrivate::transformChanged();
}

void QSGFlipable::updatePolish()
{
    Q_D(QSGFlipable);
    d->updateSide();
}

// determination on the currently visible side of the flipable
// has to be done on the complete scene transform to give
// correct results.
void QSGFlipablePrivate::updateSide()
{
    Q_Q(QSGFlipable);

    if (!sideDirty)
        return;

    sideDirty = false;

    QTransform sceneTransform;
    itemToParentTransform(sceneTransform);

    QPointF p1(0, 0);
    QPointF p2(1, 0);
    QPointF p3(1, 1);

    QPointF scenep1 = sceneTransform.map(p1);
    QPointF scenep2 = sceneTransform.map(p2);
    QPointF scenep3 = sceneTransform.map(p3);
#if 0
    p1 = q->mapToParent(p1);
    p2 = q->mapToParent(p2);
    p3 = q->mapToParent(p3);
#endif

    qreal cross = (scenep1.x() - scenep2.x()) * (scenep3.y() - scenep2.y()) -
                  (scenep1.y() - scenep2.y()) * (scenep3.x() - scenep2.x());

    wantBackYFlipped = scenep1.x() >= scenep2.x();
    wantBackXFlipped = scenep2.y() >= scenep3.y();

    QSGFlipable::Side newSide;
    if (cross > 0) {
        newSide = QSGFlipable::Back;
    } else {
        newSide = QSGFlipable::Front;
    }

    if (newSide != current) {
        current = newSide;
        if (current == QSGFlipable::Back && back)
            setBackTransform();
        if (front)
            front->setOpacity((current==QSGFlipable::Front)?1.:0.);
        if (back)
            back->setOpacity((current==QSGFlipable::Back)?1.:0.);
        emit q->sideChanged();
    }
}

/* Depends on the width/height of the back item, and so needs reevaulating
   if those change.
*/
void QSGFlipablePrivate::setBackTransform()
{
    QTransform mat;
    mat.translate(back->width()/2,back->height()/2);
    if (back->width() && wantBackYFlipped)
        mat.rotate(180, Qt::YAxis);
    if (back->height() && wantBackXFlipped)
        mat.rotate(180, Qt::XAxis);
    mat.translate(-back->width()/2,-back->height()/2);

    if (backTransform)
        backTransform->setTransform(mat);
}

QT_END_NAMESPACE

#include "qsgflipable.moc"
