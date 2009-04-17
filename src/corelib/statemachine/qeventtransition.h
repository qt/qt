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

#ifndef QEVENTTRANSITION_H
#define QEVENTTRANSITION_H

#include "qtransition.h"
#include <QtCore/qcoreevent.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QEventTransitionPrivate;
class Q_CORE_EXPORT QEventTransition : public QTransition
{
    Q_OBJECT
    Q_PROPERTY(QObject* object READ eventSource WRITE setEventSource)
    Q_PROPERTY(QEvent::Type eventType READ eventType WRITE setEventType)
public:
    QEventTransition(QState *sourceState = 0);
    QEventTransition(QObject *object, QEvent::Type type, QState *sourceState = 0);
    QEventTransition(QObject *object, QEvent::Type type,
                     const QList<QAbstractState*> &targets, QState *sourceState = 0);
    ~QEventTransition();

    QObject *eventSource() const;
    void setEventSource(QObject *object);

    QEvent::Type eventType() const;
    void setEventType(QEvent::Type type);

protected:
    virtual bool testEventCondition(QEvent *event) const; // ### name

    bool eventTest(QEvent *event) const;
    void onTransition();

    bool event(QEvent *e);

protected:
    QEventTransition(QEventTransitionPrivate &dd, QState *parent);
    QEventTransition(QEventTransitionPrivate &dd, QObject *object,
                     QEvent::Type type, QState *parent);
    QEventTransition(QEventTransitionPrivate &dd, QObject *object,
                     QEvent::Type type, const QList<QAbstractState*> &targets,
                     QState *parent);

private:
    Q_DISABLE_COPY(QEventTransition)
    Q_DECLARE_PRIVATE(QEventTransition)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
