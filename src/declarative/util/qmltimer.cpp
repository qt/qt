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

#include "QtCore/qcoreapplication.h"
#include "QtCore/qpauseanimation.h"
#include "private/qobject_p.h"
#include "qmltimer.h"
#include "qdebug.h"

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4.6,Timer,QmlTimer)

class QmlTimerPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlTimer)
public:
    QmlTimerPrivate()
        : interval(1000), running(false), repeating(false), triggeredOnStart(false)
        , componentComplete(false) {}
    int interval;
    bool running;
    bool repeating;
    bool triggeredOnStart;
    QPauseAnimation pause;
    bool componentComplete;
};

/*!
    \qmlclass Timer QFxTimer
    \brief The Timer item triggers a handler at a specified interval.

    A timer can be used to trigger an action either once, or repeatedly
    at a given interval.

    \qml
    Timer {
        interval: 500; running: true; repeat: true
        onTriggered: Time.text = Date().toString()
    }
    Text {
        id: Time
    }
    \endqml

*/

QmlTimer::QmlTimer(QObject *parent)
    : QObject(*(new QmlTimerPrivate), parent)
{
    Q_D(QmlTimer);
    connect(&d->pause, SIGNAL(currentLoopChanged(int)), this, SLOT(ticked()));
    connect(&d->pause, SIGNAL(finished()), this, SLOT(ticked()));
    connect(&d->pause, SIGNAL(stateChanged(QAbstractAnimation::State,QAbstractAnimation::State))
            , this, SLOT(stateChanged(QAbstractAnimation::State,QAbstractAnimation::State)));
    d->pause.setLoopCount(1);
    d->pause.setDuration(d->interval);
}

/*!
    \qmlproperty int Timer::interval

    Sets the \a interval in milliseconds between triggering.
*/
void QmlTimer::setInterval(int interval)
{
    Q_D(QmlTimer);
    if (interval != d->interval) {
        d->interval = interval;
        update();
    }
}

int QmlTimer::interval() const
{
    Q_D(const QmlTimer);
    return d->interval;
}

/*!
    \qmlproperty bool Timer::running

    If set to true, starts the timer; otherwise stops the timer.
    For a non-repeating timer, \a running  will be set to false after the
    timer has been triggered.

    \sa repeat
*/
bool QmlTimer::isRunning() const
{
    Q_D(const QmlTimer);
    return d->running;
}

void QmlTimer::setRunning(bool running)
{
    Q_D(QmlTimer);
    if (d->running != running) {
        d->running = running;
        emit runningChanged();
        update();
    }
}

/*!
    \qmlproperty bool Timer::repeat

    If \a repeat is true the timer will be triggered repeatedly at the
    specified interval; otherwise, the timer will trigger once at the
    specified interval and then stop (i.e. running will be set to false).

    \sa running
*/
bool QmlTimer::isRepeating() const
{
    Q_D(const QmlTimer);
    return d->repeating;
}

void QmlTimer::setRepeating(bool repeating)
{
    Q_D(QmlTimer);
    if (repeating != d->repeating) {
        d->repeating = repeating;
        update();
    }
}

/*!
    \qmlproperty bool Timer::triggeredOnStart

    If \a triggeredOnStart is true, the timer will be triggered immediately
    when started, and subsequently at the specified interval.

    \sa running
*/
bool QmlTimer::triggeredOnStart() const
{
    Q_D(const QmlTimer);
    return d->triggeredOnStart;
}

void QmlTimer::setTriggeredOnStart(bool triggeredOnStart)
{
    Q_D(QmlTimer);
    if (d->triggeredOnStart != triggeredOnStart) {
        d->triggeredOnStart = triggeredOnStart;
        update();
    }
}

void QmlTimer::update()
{
    Q_D(QmlTimer);
    if (!d->componentComplete)
        return;
    d->pause.stop();
    if (d->running) {
        d->pause.setLoopCount(d->repeating ? -1 : 1);
        d->pause.setDuration(d->interval);
        d->pause.start();
        if (d->triggeredOnStart) {
            QCoreApplication::removePostedEvents(this, QEvent::MetaCall);
            QMetaObject::invokeMethod(this, "ticked", Qt::QueuedConnection);
        }
    }
}

void QmlTimer::componentComplete()
{
    Q_D(QmlTimer);
    d->componentComplete = true;
    update();
}

/*!
    \qmlsignal Timer::onTriggered

    This handler is called when the Timer is triggered.
*/
void QmlTimer::ticked()
{
    emit triggered();
}

void QmlTimer::stateChanged(QAbstractAnimation::State, QAbstractAnimation::State state)
{
    Q_D(QmlTimer);
    if (d->running && state != QAbstractAnimation::Running) {
        d->running = false;
        emit runningChanged();
    }
}

QT_END_NAMESPACE
