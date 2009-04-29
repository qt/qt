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

#include "qbasickeyeventtransition_p.h"
#include <QtGui/qevent.h>
#include <qdebug.h>

#if defined(QT_EXPERIMENTAL_SOLUTION)
# include "qabstracttransition_p.h"
#else
# include <private/qabstracttransition_p.h>
#endif


QT_BEGIN_NAMESPACE

/*!
  \internal
  \class QBasicKeyEventTransition

  \brief The QBasicKeyEventTransition class provides a transition for Qt key events.
*/

class QBasicKeyEventTransitionPrivate : public QAbstractTransitionPrivate
{
    Q_DECLARE_PUBLIC(QBasicKeyEventTransition)
public:
    QBasicKeyEventTransitionPrivate();

    static QBasicKeyEventTransitionPrivate *get(QBasicKeyEventTransition *q);

    QEvent::Type eventType;
    int key;
    Qt::KeyboardModifiers modifiersMask;
};

QBasicKeyEventTransitionPrivate::QBasicKeyEventTransitionPrivate()
{
    eventType = QEvent::None;
    key = 0;
    modifiersMask = Qt::NoModifier;
}

QBasicKeyEventTransitionPrivate *QBasicKeyEventTransitionPrivate::get(QBasicKeyEventTransition *q)
{
    return q->d_func();
}

/*!
  Constructs a new key event transition with the given \a sourceState.
*/
QBasicKeyEventTransition::QBasicKeyEventTransition(QState *sourceState)
    : QAbstractTransition(*new QBasicKeyEventTransitionPrivate, sourceState)
{
}

/*!
  Constructs a new event transition for events of the given \a type for the
  given \a key, with the given \a sourceState.
*/
QBasicKeyEventTransition::QBasicKeyEventTransition(QEvent::Type type, int key,
                                                   QState *sourceState)
    : QAbstractTransition(*new QBasicKeyEventTransitionPrivate, sourceState)
{
    Q_D(QBasicKeyEventTransition);
    d->eventType = type;
    d->key = key;
}

/*!
  Constructs a new event transition for events of the given \a type for the
  given \a key, with the given \a modifiersMask and \a sourceState.
*/
QBasicKeyEventTransition::QBasicKeyEventTransition(QEvent::Type type, int key,
                                                   Qt::KeyboardModifiers modifiersMask,
                                                   QState *sourceState)
    : QAbstractTransition(*new QBasicKeyEventTransitionPrivate, sourceState)
{
    Q_D(QBasicKeyEventTransition);
    d->eventType = type;
    d->key = key;
    d->modifiersMask = modifiersMask;
}

/*!
  Destroys this event transition.
*/
QBasicKeyEventTransition::~QBasicKeyEventTransition()
{
}

/*!
  Returns the event type that this key event transition is associated with.
*/
QEvent::Type QBasicKeyEventTransition::eventType() const
{
    Q_D(const QBasicKeyEventTransition);
    return d->eventType;
}

/*!
  Sets the event \a type that this key event transition is associated with.
*/
void QBasicKeyEventTransition::setEventType(QEvent::Type type)
{
    Q_D(QBasicKeyEventTransition);
    d->eventType = type;
}

/*!
  Returns the key that this key event transition checks for.
*/
int QBasicKeyEventTransition::key() const
{
    Q_D(const QBasicKeyEventTransition);
    return d->key;
}

/*!
  Sets the key that this key event transition will check for.
*/
void QBasicKeyEventTransition::setKey(int key)
{
    Q_D(QBasicKeyEventTransition);
    d->key = key;
}

/*!
  Returns the keyboard modifiers mask that this key event transition checks
  for.
*/
Qt::KeyboardModifiers QBasicKeyEventTransition::modifiersMask() const
{
    Q_D(const QBasicKeyEventTransition);
    return d->modifiersMask;
}

/*!
  Sets the keyboard modifiers mask that this key event transition will check
  for.
*/
void QBasicKeyEventTransition::setModifiersMask(Qt::KeyboardModifiers modifiersMask)
{
    Q_D(QBasicKeyEventTransition);
    d->modifiersMask = modifiersMask;
}

/*!
  \reimp
*/
bool QBasicKeyEventTransition::eventTest(QEvent *event) const
{
    Q_D(const QBasicKeyEventTransition);
    if (event->type() == d->eventType) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        return (ke->key() == d->key)
            && ((ke->modifiers() & d->modifiersMask) == d->modifiersMask);
    }
    return false;
}

/*!
  \reimp
*/
void QBasicKeyEventTransition::onTransition()
{
}

QT_END_NAMESPACE
