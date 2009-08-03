#include "tapandholdgesture.h"

#include <QtGui/qevent.h>

/*!
    \class TapAndHoldGesture
    \since 4.6

    \brief The TapAndHoldGesture class represents a Tap-and-Hold gesture,
    providing additional information.
*/

const int TapAndHoldGesture::iterationCount = 40;
const int TapAndHoldGesture::iterationTimeout = 50;

/*!
    Creates a new Tap and Hold gesture handler object and marks it as a child
    of \a parent.

    On some platforms like Windows there is a system-wide tap and hold gesture
    that cannot be overriden, hence the gesture might never trigger and default
    context menu will be shown instead.
*/
TapAndHoldGesture::TapAndHoldGesture(QWidget *parent)
    : QGesture(parent), iteration(0)
{
}

/*! \internal */
bool TapAndHoldGesture::filterEvent(QEvent *event)
{
    if (!event->spontaneous())
        return false;
    const QTouchEvent *ev = static_cast<const QTouchEvent*>(event);
    switch (event->type()) {
    case QEvent::TouchBegin: {
        if (timer.isActive())
            timer.stop();
        timer.start(TapAndHoldGesture::iterationTimeout, this);
        const QPoint p = ev->touchPoints().at(0).pos().toPoint();
        position = p;
        break;
    }
    case QEvent::TouchUpdate:
        if (ev->touchPoints().size() == 1) {
            const QPoint startPos = ev->touchPoints().at(0).startPos().toPoint();
            const QPoint pos = ev->touchPoints().at(0).pos().toPoint();
            if ((startPos - pos).manhattanLength() > 15)
                reset();
        } else {
            reset();
        }
        break;
    case QEvent::TouchEnd:
        reset();
        break;
    default:
        break;
    }
    return false;
}

/*! \internal */
void TapAndHoldGesture::timerEvent(QTimerEvent *event)
{
    if (event->timerId() != timer.timerId())
        return;
    if (iteration == TapAndHoldGesture::iterationCount) {
        timer.stop();
        setState(Qt::GestureFinished);
        emit triggered();
    } else {
        setState(Qt::GestureStarted);
        emit triggered();
    }
    ++iteration;
}

/*! \internal */
void TapAndHoldGesture::reset()
{
    if (state() != Qt::NoGesture)
        emit cancelled();
    setState(Qt::NoGesture);
    timer.stop();
    iteration = 0;
}

/*!
    \property TapAndHoldGesture::pos

    \brief The position of the gesture.
*/
QPoint TapAndHoldGesture::pos() const
{
    return position;
}
