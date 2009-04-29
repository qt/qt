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

#include "qkeyeventtransition.h"
#include "qbasickeyeventtransition_p.h"
#include <QtCore/qwrappedevent.h>

#if defined(QT_EXPERIMENTAL_SOLUTION)
# include "qeventtransition_p.h"
#else
# include <private/qeventtransition_p.h>
#endif

QT_BEGIN_NAMESPACE

/*!
  \class QKeyEventTransition

  \brief The QKeyEventTransition class provides a transition for key events.

  \since 4.6
  \ingroup statemachine

  QKeyEventTransition is part of \l{The State Machine Framework}.

  \sa QState::addTransition()
*/

/*!
    \property QKeyEventTransition::key

    \brief the key that this key event transition is associated with
*/

/*!
    \property QKeyEventTransition::modifiersMask

    \brief the keyboard modifiers mask that this key event transition checks for
*/

class QKeyEventTransitionPrivate : public QEventTransitionPrivate
{
    Q_DECLARE_PUBLIC(QKeyEventTransition)
public:
    QKeyEventTransitionPrivate() {}

    QBasicKeyEventTransition *transition;
};

/*!
  Constructs a new key event transition with the given \a sourceState.
*/
QKeyEventTransition::QKeyEventTransition(QState *sourceState)
    : QEventTransition(*new QKeyEventTransitionPrivate, sourceState)
{
    Q_D(QKeyEventTransition);
    d->transition = new QBasicKeyEventTransition();
}

/*!
  Constructs a new key event transition for events of the given \a type for
  the given \a object, with the given \a key and \a sourceState.
*/
QKeyEventTransition::QKeyEventTransition(QObject *object, QEvent::Type type,
                                         int key, QState *sourceState)
    : QEventTransition(*new QKeyEventTransitionPrivate, object, type, sourceState)
{
    Q_D(QKeyEventTransition);
    d->transition = new QBasicKeyEventTransition(type, key);
}

/*!
  Constructs a new key event transition for events of the given \a type for
  the given \a object, with the given \a key, \a targets and \a sourceState.
*/
QKeyEventTransition::QKeyEventTransition(QObject *object, QEvent::Type type,
                                         int key, const QList<QAbstractState*> &targets,
                                         QState *sourceState)
    : QEventTransition(*new QKeyEventTransitionPrivate, object, type, targets, sourceState)
{
    Q_D(QKeyEventTransition);
    d->transition = new QBasicKeyEventTransition(type, key);
}

/*!
  Destroys this key event transition.
*/
QKeyEventTransition::~QKeyEventTransition()
{
    Q_D(QKeyEventTransition);
    delete d->transition;
}

/*!
  Returns the key that this key event transition checks for.
*/
int QKeyEventTransition::key() const
{
    Q_D(const QKeyEventTransition);
    return d->transition->key();
}

/*!
  Sets the key that this key event transition will check for.
*/
void QKeyEventTransition::setKey(int key)
{
    Q_D(QKeyEventTransition);
    d->transition->setKey(key);
}

/*!
  Returns the keyboard modifiers mask that this key event transition checks
  for.
*/
Qt::KeyboardModifiers QKeyEventTransition::modifiersMask() const
{
    Q_D(const QKeyEventTransition);
    return d->transition->modifiersMask();
}

/*!
  Sets the keyboard \a modifiers mask that this key event transition will
  check for.
*/
void QKeyEventTransition::setModifiersMask(Qt::KeyboardModifiers modifiersMask)
{
    Q_D(QKeyEventTransition);
    d->transition->setModifiersMask(modifiersMask);
}

/*!
  \reimp
*/
bool QKeyEventTransition::eventTest(QEvent *event) const
{
    Q_D(const QKeyEventTransition);
    if (!QEventTransition::eventTest(event))
        return false;
    QWrappedEvent *we = static_cast<QWrappedEvent*>(event);
    d->transition->setEventType(we->event()->type());
    return QAbstractTransitionPrivate::get(d->transition)->callEventTest(we->event());
}

/*!
  \reimp
*/
void QKeyEventTransition::onTransition()
{
    QEventTransition::onTransition();
}

QT_END_NAMESPACE
