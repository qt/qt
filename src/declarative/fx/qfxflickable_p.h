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

#ifndef QFXFLICKABLE_P_H
#define QFXFLICKABLE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qdatetime.h"
#include "qfxflickable.h"
#include "qfxitem_p.h"
#include "qml.h"
#include "private/qmltimeline_p.h"
#include "private/qmlanimation_p.h"

QT_BEGIN_NAMESPACE

class ElasticValue : public QAbstractAnimation {
    Q_OBJECT
public:
    ElasticValue(QmlTimeLineValue &);
    void setValue(qreal to);
    void clear();

    virtual int duration() const { return 10000; }

protected:
    virtual void updateCurrentTime(int);

Q_SIGNALS:
    void updated();

private:
    qreal _to;
    qreal _myValue;
    qreal _velocity;
    QmlTimeLineValue &_value;
    QTime _startTime;
};

class QFxFlickablePrivate : public QFxItemPrivate
{
    Q_DECLARE_PUBLIC(QFxFlickable)

public:
    QFxFlickablePrivate();
    void init();
    virtual void fixupX();
    virtual void fixupY();
    void updateBeginningEnd();

public:
    QFxItem *_flick;
    QmlTimeLineValueProxy<QFxItem> _moveX;
    QmlTimeLineValueProxy<QFxItem> _moveY;
    QmlTimeLine _tl;
    int vWidth;
    int vHeight;
    bool overShoot;
    bool flicked;
    bool moving;
    bool stealMouse;
    bool pressed;
    QTime lastPosTime;
    QPointF lastPos;
    QPointF pressPos;
    qreal pressX;
    qreal pressY;
    qreal velocityX;
    qreal velocityY;
    QTime pressTime;
    QmlTimeLineEvent fixupXEvent;
    QmlTimeLineEvent fixupYEvent;
    int maxVelocity;
    bool locked;
    QFxFlickable::DragMode dragMode;
    ElasticValue elasticY;
    ElasticValue elasticX;
    QTime velocityTime;
    QPointF lastFlickablePosition;
    int velocityDecay;

    void updateVelocity();
    struct Velocity : public QmlTimeLineValue
    {
        Velocity(QFxFlickablePrivate *p)
            : parent(p) {}
        virtual void setValue(qreal v) {
            QmlTimeLineValue::setValue(v);
            parent->updateVelocity();
        }
        QFxFlickablePrivate *parent;
    };
    Velocity xVelocity;
    Velocity yVelocity;
    int vTime;
    QmlTimeLine velocityTimeline;
    bool atXEnd;
    bool atXBeginning;
    qreal pageXPosition;
    qreal pageWidth;
    bool atYEnd;
    bool atYBeginning;
    qreal pageYPosition;
    qreal pageHeight;

    void handleMousePressEvent(QGraphicsSceneMouseEvent *);
    void handleMouseMoveEvent(QGraphicsSceneMouseEvent *);
    void handleMouseReleaseEvent(QGraphicsSceneMouseEvent *);

    // flickableData property
    void data_removeAt(int);
    int data_count() const;
    void data_append(QObject *);
    void data_insert(int, QObject *);
    QObject *data_at(int) const;
    void data_clear();
    QML_DECLARE_LIST_PROXY(QFxFlickablePrivate, QObject *, data)
};

QT_END_NAMESPACE

#endif
