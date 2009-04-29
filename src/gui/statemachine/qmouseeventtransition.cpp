/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include "qmouseeventtransition.h"
#include "qbasicmouseeventtransition_p.h"
#include <QtGui/qpainterpath.h>

#if defined(QT_EXPERIMENTAL_SOLUTION)
# include "qeventtransition_p.h"
#else
# include <private/qeventtransition_p.h>
#endif

QT_BEGIN_NAMESPACE

/*!
  \class QMouseEventTransition

  \brief The QMouseEventTransition class provides a transition for mouse events.

  \since 4.6
  \ingroup statemachine

  QMouseEventTransition is part of \l{The State Machine Framework}.

  \sa QState::addTransition()
*/

/*!
    \property QMouseEventTransition::button

    \brief the button that this mouse event transition is associated with
*/

/*!
    \property QMouseEventTransition::modifiersMask

    \brief the keyboard modifiers mask that this mouse event transition checks for
*/

class QMouseEventTransitionPrivate : public QEventTransitionPrivate
{
    Q_DECLARE_PUBLIC(QMouseEventTransition)
public:
    QMouseEventTransitionPrivate();

    QBasicMouseEventTransition *transition;
};

QMouseEventTransitionPrivate::QMouseEventTransitionPrivate()
{
}

/*!
  Constructs a new mouse event transition with the given \a sourceState.
*/
QMouseEventTransition::QMouseEventTransition(QState *sourceState)
    : QEventTransition(*new QMouseEventTransitionPrivate, sourceState)
{
    Q_D(QMouseEventTransition);
    d->transition = new QBasicMouseEventTransition();
}

/*!
  Constructs a new mouse event transition for events of the given \a type for
  the given \a object, with the given \a button and \a sourceState.
*/
QMouseEventTransition::QMouseEventTransition(QObject *object, QEvent::Type type,
                                             Qt::MouseButton button,
                                             QState *sourceState)
    : QEventTransition(*new QMouseEventTransitionPrivate, object, type, sourceState)
{
    Q_D(QMouseEventTransition);
    d->transition = new QBasicMouseEventTransition(type, button);
}

/*!
  Constructs a new mouse event transition for events of the given \a type for
  the given \a object, with the given \a button, \a targets and \a
  sourceState.
*/
QMouseEventTransition::QMouseEventTransition(QObject *object, QEvent::Type type,
                                             Qt::MouseButton button,
                                             const QList<QAbstractState*> &targets,
                                             QState *sourceState)
    : QEventTransition(*new QMouseEventTransitionPrivate, object, type, targets, sourceState)
{
    Q_D(QMouseEventTransition);
    d->transition = new QBasicMouseEventTransition(type, button);
}

/*!
  Destroys this mouse event transition.
*/
QMouseEventTransition::~QMouseEventTransition()
{
    Q_D(QMouseEventTransition);
    delete d->transition;
}

/*!
  Returns the button that this mouse event transition checks for.
*/
Qt::MouseButton QMouseEventTransition::button() const
{
    Q_D(const QMouseEventTransition);
    return d->transition->button();
}

/*!
  Sets the \a button that this mouse event transition will check for.
*/
void QMouseEventTransition::setButton(Qt::MouseButton button)
{
    Q_D(QMouseEventTransition);
    d->transition->setButton(button);
}

/*!
  Returns the keyboard modifiers mask that this mouse event transition checks
  for.
*/
Qt::KeyboardModifiers QMouseEventTransition::modifiersMask() const
{
    Q_D(const QMouseEventTransition);
    return d->transition->modifiersMask();
}

/*!
  Sets the keyboard \a modifiers mask that this mouse event transition will
  check for.
*/
void QMouseEventTransition::setModifiersMask(Qt::KeyboardModifiers modifiersMask)
{
    Q_D(QMouseEventTransition);
    d->transition->setModifiersMask(modifiersMask);
}

/*!
  Returns the path for this mouse event transition.
*/
QPainterPath QMouseEventTransition::path() const
{
    Q_D(const QMouseEventTransition);
    return d->transition->path();
}

/*!
  Sets the \a path for this mouse event transition.
  If a valid path has been set, the transition will only trigger if the mouse
  event position (QMouseEvent::pos()) is inside the path.

  \sa QPainterPath::contains()
*/
void QMouseEventTransition::setPath(const QPainterPath &path)
{
    Q_D(QMouseEventTransition);
    d->transition->setPath(path);
}

/*!
  \reimp
*/
bool QMouseEventTransition::testEventCondition(QEvent *event) const
{
    Q_D(const QMouseEventTransition);
    d->transition->setEventType(event->type());
    return QAbstractTransitionPrivate::get(d->transition)->callEventTest(event);
}

/*!
  \reimp
*/
bool QMouseEventTransition::eventTest(QEvent *event) const
{
    return QEventTransition::eventTest(event);
}

/*!
  \reimp
*/
void QMouseEventTransition::onTransition()
{
    QEventTransition::onTransition();
}

QT_END_NAMESPACE
