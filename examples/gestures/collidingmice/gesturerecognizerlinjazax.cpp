#include "gesturerecognizerlinjazax.h"

#include <QRegExp>
#include <QDebug>

static const int SIZE = 20;

DirectionSimpleRecognizer::DirectionSimpleRecognizer()
{
}

Direction DirectionSimpleRecognizer::addPosition(const QPoint &pos)
{
    if (!directions.isEmpty()) {
        const QPoint tmp = pos - directions.back().point;
        if (tmp.manhattanLength() < 5)
            return Direction();
    }
    if (lastPoint.isNull()) {
        lastPoint = pos;
        return Direction();
    }
    int dx = pos.x() - lastPoint.x();
    int dy = pos.y() - lastPoint.y();
    QString direction;
    if (dx < 0) {
        if (-1*dx >= SIZE/2)
            direction = "4";
    } else {
        if (dx >= SIZE/2)
            direction = "6";
    }
    if (dy < 0) {
        if (-1*dy >= SIZE/2)
            direction = "8";
    } else {
        if (dy >= SIZE/2)
            direction = "2";
    }
    if (direction.isEmpty())
        return Direction();

    lastPoint = pos;
    directions.push_back(Direction(direction, pos));
    return Direction(direction, pos);
}


DirectionList DirectionSimpleRecognizer::getDirections() const
{
    return directions;
}

void DirectionSimpleRecognizer::reset()
{
    directions.clear();
    lastPoint = QPoint();
}

///////////////////////////////////////////////////////////////////////////

GestureRecognizerLinjaZax::GestureRecognizerLinjaZax()
    : QGestureRecognizer("LinjaZax"), mousePressed(false), gestureFinished(false),
      zoomState(LinjaZaxGesture::NoZoom)
{
}

QGestureRecognizer::Result GestureRecognizerLinjaZax::recognize(const QList<QEvent*> &inputEvents)
{
    // get all mouse events
    QList<QMouseEvent*> events;
    for(int i = 0; i < inputEvents.count(); ++i) {
        QEvent *event = inputEvents.at(i);
        switch (event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
            events.push_back(static_cast<QMouseEvent*>(event));
        default:
            break;
        }
    }

    if (zoomState != LinjaZaxGesture::NoZoom && !lastDirections.isEmpty()) {
        lastDirections = lastDirections.right(1);
        zoomState = LinjaZaxGesture::NoZoom;
    }

    QGestureRecognizer::Result result = QGestureRecognizer::NotGesture;
    for(int i = 0; i < events.count(); ++i) {
        QMouseEvent *event = events.at(i);
        if (event->type() == QEvent::MouseButtonPress) {
            if (!currentDirection.isEmpty()) {
                result = QGestureRecognizer::NotGesture;
                reset();
                break;
            }
            result = QGestureRecognizer::MaybeGesture;
            mousePressed = true;
            pressedPos = lastPos = currentPos = event->pos();
        } else if (event->type() == QEvent::MouseButtonRelease) {
            if (mousePressed && !currentDirection.isEmpty()) {
                result = QGestureRecognizer::GestureFinished;
                gestureFinished = true;
                currentPos = event->pos();
                internalReset();
                break;
            }
            result = QGestureRecognizer::NotGesture;
            reset();
            break;
        } else if (event->type() == QEvent::MouseMove) {
            if (!mousePressed)
                continue;
            lastPos = currentPos;
            currentPos = event->pos();
            QString direction =
                simpleRecognizer.addPosition(event->pos()).direction;
            if (currentDirection.isEmpty()) {
                if (direction.isEmpty())
                    result = QGestureRecognizer::MaybeGesture;
                else
                    result = QGestureRecognizer::GestureStarted;
            } else {
                result = QGestureRecognizer::GestureStarted;
            }
            if (!direction.isEmpty()) {
                lastDirections.append(direction);
                currentDirection = direction;
                if (lastDirections.length() > 5)
                    lastDirections.remove(0, 1);
                if (lastDirections.contains("248") || lastDirections.contains("2448"))
                    zoomState = LinjaZaxGesture::ZoomingIn;
                else if (lastDirections.contains("268") || lastDirections.contains("2668"))
                    zoomState = LinjaZaxGesture::ZoomingOut;
            }
        }
    }
    return result;
}

static inline LinjaZaxGesture::DirectionType convertPanningDirection(const QString &direction)
{
    if (direction.length() == 1) {
        if (direction == "4")
            return LinjaZaxGesture::Left;
        else if (direction == "6")
            return LinjaZaxGesture::Right;
        else if (direction == "8")
            return LinjaZaxGesture::Up;
        else if (direction == "2")
            return LinjaZaxGesture::Down;
    }
    return LinjaZaxGesture::None;
}

QGesture* GestureRecognizerLinjaZax::makeEvent() const
{
    LinjaZaxGesture::DirectionType dir = convertPanningDirection(currentDirection);
    LinjaZaxGesture::DirectionType lastDir = convertPanningDirection(lastDirections.right(1));
    if (dir == LinjaZaxGesture::None)
        return 0;
    LinjaZaxGesture *g =
        new LinjaZaxGesture("LinjaZax", pressedPos, lastPos, currentPos,
                            QRect(), pressedPos, QDateTime(), 0,
                            gestureFinished ? Qt::GestureFinished : Qt::GestureStarted);
    g->lastDirection_ = lastDir;
    g->direction_ = dir;
    g->zoomState_ = zoomState;
    
    return g;
}

void GestureRecognizerLinjaZax::reset()
{
    mousePressed = false;
    lastDirections.clear();
    currentDirection.clear();
    gestureFinished = false;
    simpleRecognizer.reset();
    zoomState = LinjaZaxGesture::NoZoom;
}

void GestureRecognizerLinjaZax::internalReset()
{
    mousePressed = false;
    simpleRecognizer.reset();
}
