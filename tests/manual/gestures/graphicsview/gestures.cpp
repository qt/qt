#include "gestures.h"

#include <QTouchEvent>

Qt::GestureType ThreeFingerSlideGesture::Type = Qt::CustomGesture;

QGesture *ThreeFingerSlideGestureRecognizer::createGesture(QObject *)
{
    return new ThreeFingerSlideGesture;
}

QGestureRecognizer::Result ThreeFingerSlideGestureRecognizer::filterEvent(QGesture *state, QObject *, QEvent *event)
{
    ThreeFingerSlideGesture *d = static_cast<ThreeFingerSlideGesture *>(state);
    QGestureRecognizer::Result result;
    switch (event->type()) {
    case QEvent::TouchBegin:
        result = QGestureRecognizer::MaybeGesture;
    case QEvent::TouchEnd:
        if (d->gestureFired)
            result = QGestureRecognizer::GestureFinished;
        else
            result = QGestureRecognizer::NotGesture;
    case QEvent::TouchUpdate:
        if (d->state() != Qt::NoGesture) {
            QTouchEvent *ev = static_cast<QTouchEvent*>(event);
            if (ev->touchPoints().size() == 3) {
                d->gestureFired = true;
                result = QGestureRecognizer::GestureTriggered;
            } else {
                result = QGestureRecognizer::MaybeGesture;
                for (int i = 0; i < ev->touchPoints().size(); ++i) {
                    const QTouchEvent::TouchPoint &pt = ev->touchPoints().at(i);
                    const int distance = (pt.pos().toPoint() - pt.startPos().toPoint()).manhattanLength();
                    if (distance > 20) {
                        result = QGestureRecognizer::NotGesture;
                    }
                }
            }
        } else {
            result = QGestureRecognizer::NotGesture;
        }

        break;
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
        if (d->state() != Qt::NoGesture)
            result = QGestureRecognizer::Ignore;
        else
            result = QGestureRecognizer::NotGesture;
        break;
    default:
        result = QGestureRecognizer::Ignore;
        break;
    }
    return result;
}

void ThreeFingerSlideGestureRecognizer::reset(QGesture *state)
{
    static_cast<ThreeFingerSlideGesture *>(state)->gestureFired = false;
    QGestureRecognizer::reset(state);
}


QGesture *RotateGestureRecognizer::createGesture(QObject *)
{
    return new QGesture;
}

QGestureRecognizer::Result RotateGestureRecognizer::filterEvent(QGesture *, QObject *, QEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchEnd:
    case QEvent::TouchUpdate:
        break;
    default:
        break;
    }
    return QGestureRecognizer::Ignore;
}

void RotateGestureRecognizer::reset(QGesture *state)
{
    QGestureRecognizer::reset(state);
}

#include "moc_gestures.cpp"
