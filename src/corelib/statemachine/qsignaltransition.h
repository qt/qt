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

#ifndef QSIGNALTRANSITION_H
#define QSIGNALTRANSITION_H

#ifndef QT_STATEMACHINE_SOLUTION
#include <QtCore/qtransition.h>
#else
#include "qtransition.h"
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QSignalTransitionPrivate;
class Q_CORE_EXPORT QSignalTransition : public QTransition
{
    Q_OBJECT
    Q_PROPERTY(QObject* object READ senderObject WRITE setSenderObject)
    Q_PROPERTY(QByteArray signal READ signal WRITE setSignal)
public:
    QSignalTransition(QState *sourceState = 0);
    QSignalTransition(QObject *sender, const char *signal,
                      QState *sourceState = 0);
    QSignalTransition(QObject *sender, const char *signal,
                      const QList<QAbstractState*> &targets,
                      QState *sourceState = 0);
    ~QSignalTransition();

    QObject *senderObject() const;
    void setSenderObject(QObject *sender);

    QByteArray signal() const;
    void setSignal(const QByteArray &signal);

protected:
    bool eventTest(QEvent *event) const;
    void onTransition();

    bool event(QEvent *e);

private:
    Q_DISABLE_COPY(QSignalTransition)
    Q_DECLARE_PRIVATE(QSignalTransition)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
