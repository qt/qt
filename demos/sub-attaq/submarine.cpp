/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

//Own
#include "submarine.h"
#include "submarine_p.h"
#include "torpedo.h"
#include "pixmapitem.h"
#include "graphicsscene.h"
#include "animationmanager.h"
#include "custompropertyanimation.h"
#include "qanimationstate.h"

#include <QtCore/QPropertyAnimation>
#include <QtCore/QStateMachine>
#include <QtCore/QFinalState>
#include <QtCore/QSequentialAnimationGroup>

static QAbstractAnimation *setupDestroyAnimation(SubMarine *sub)
{
    QSequentialAnimationGroup *group = new QSequentialAnimationGroup(sub);
#if QT_VERSION >=0x040500
    PixmapItem *step1 = new PixmapItem(QString("explosion/submarine/step1"),GraphicsScene::Big, sub);
    step1->setZValue(6);
    PixmapItem *step2 = new PixmapItem(QString("explosion/submarine/step2"),GraphicsScene::Big, sub);
    step2->setZValue(6);
    PixmapItem *step3 = new PixmapItem(QString("explosion/submarine/step3"),GraphicsScene::Big, sub);
    step3->setZValue(6);
    PixmapItem *step4 = new PixmapItem(QString("explosion/submarine/step4"),GraphicsScene::Big, sub);
    step4->setZValue(6);
    step1->setOpacity(0);
    step2->setOpacity(0);
    step3->setOpacity(0);
    step4->setOpacity(0);
    CustomPropertyAnimation *anim1 = new CustomPropertyAnimation(sub);
    anim1->setMemberFunctions((QGraphicsItem*)step1, &QGraphicsItem::opacity, &QGraphicsItem::setOpacity);
    anim1->setDuration(100);
    anim1->setEndValue(1);
    CustomPropertyAnimation *anim2 = new CustomPropertyAnimation(sub);
    anim2->setMemberFunctions((QGraphicsItem*)step2, &QGraphicsItem::opacity, &QGraphicsItem::setOpacity);
    anim2->setDuration(100);
    anim2->setEndValue(1);
    CustomPropertyAnimation *anim3 = new CustomPropertyAnimation(sub);
    anim3->setMemberFunctions((QGraphicsItem*)step3, &QGraphicsItem::opacity, &QGraphicsItem::setOpacity);
    anim3->setDuration(100);
    anim3->setEndValue(1);
    CustomPropertyAnimation *anim4 = new CustomPropertyAnimation(sub);
    anim4->setMemberFunctions((QGraphicsItem*)step4, &QGraphicsItem::opacity, &QGraphicsItem::setOpacity);
    anim4->setDuration(100);
    anim4->setEndValue(1);
    group->addAnimation(anim1);
    group->addAnimation(anim2);
    group->addAnimation(anim3);
    group->addAnimation(anim4);
#else
    // work around for a bug where we don't transition if the duration is zero.
    QtPauseAnimation *anim = new QtPauseAnimation(group);
    anim->setDuration(1);
    group->addAnimation(anim);
#endif
    AnimationManager::self()->registerAnimation(group);
    return group;
}


SubMarine::SubMarine(int type, const QString &name, int points, QGraphicsItem * parent, Qt::WindowFlags wFlags)
    : QGraphicsWidget(parent,wFlags), subType(type), subName(name), subPoints(points), speed(0), direction(SubMarine::None)
{
    pixmapItem = new PixmapItem(QString("submarine"),GraphicsScene::Big, this);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setZValue(5);
    setFlags(QGraphicsItem::ItemIsMovable);
    resize(pixmapItem->boundingRect().width(),pixmapItem->boundingRect().height());
    setTransformOriginPoint(boundingRect().center());

    graphicsRotation = new QGraphicsRotation3D(this);
    graphicsRotation->setAxis(QVector3D(0, 1, 0));
    graphicsRotation->setOrigin(QPointF(size().width()/2, size().height()/2));
    QList<QGraphicsTransform *> r;
    r.append(graphicsRotation);
    setTransformations(r);

    //We setup the state machine of the submarine
    QStateMachine *machine = new QStateMachine(this);

    //This state is when the boat is moving/rotating
    QState *moving = new QState(machine);

    //This state is when the boat is moving from left to right
    MovementState *movement = new MovementState(this, moving);

    //This state is when the boat is moving from left to right
    ReturnState *rotation = new ReturnState(this, moving);

    //This is the initial state of the moving root state
    moving->setInitialState(movement);

    movement->addTransition(this, SIGNAL(subMarineStateChanged()), moving);

    //This is the initial state of the machine
    machine->setInitialState(moving);

    //End
    QFinalState *final = new QFinalState(machine);

    //If the moving animation is finished we move to the return state
    movement->addTransition(movement, SIGNAL(animationFinished()), rotation);

    //If the return animation is finished we move to the moving state
    rotation->addTransition(rotation, SIGNAL(animationFinished()), movement);

    //This state play the destroyed animation
    QAnimationState *destroyedState = new QAnimationState(machine);
    destroyedState->setAnimation(setupDestroyAnimation(this));

    //Play a nice animation when the submarine is destroyed
    moving->addTransition(this, SIGNAL(subMarineDestroyed()), destroyedState);

    //Transition to final state when the destroyed animation is finished
    destroyedState->addTransition(destroyedState, SIGNAL(animationFinished()), final);

    //The machine has finished to be executed, then the submarine is dead
    connect(machine,SIGNAL(finished()),this, SIGNAL(subMarineExecutionFinished()));

    machine->start();
}

int SubMarine::points()
{
    return subPoints;
}

void SubMarine::setCurrentDirection(SubMarine::Movement direction)
{
    if (this->direction == direction)
        return;
    if (direction == SubMarine::Right && this->direction == SubMarine::None) {
          graphicsRotation->setAngle(180);
    }
    this->direction = direction;
}

enum SubMarine::Movement SubMarine::currentDirection() const
{
    return direction;
}

void SubMarine::setCurrentSpeed(int speed)
{
    if (speed < 0 || speed > 3) {
        qWarning("SubMarine::setCurrentSpeed : The speed is invalid");
    }
    this->speed = speed;
    emit subMarineStateChanged();
}

int SubMarine::currentSpeed() const
{
    return speed;
}

void SubMarine::launchTorpedo(int speed)
{
    Torpedo * torp = new Torpedo();
    GraphicsScene *scene = static_cast<GraphicsScene *>(this->scene());
    scene->addItem(torp);
    torp->setPos(x(), y());
    torp->setCurrentSpeed(speed);
    torp->launch();
}

void SubMarine::destroy()
{
    emit subMarineDestroyed();
}

int SubMarine::type() const
{
    return Type;
}
