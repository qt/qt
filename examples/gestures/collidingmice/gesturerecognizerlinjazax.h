#ifndef GESTURERECOGNIZERLINJAZAX_H
#define GESTURERECOGNIZERLINJAZAX_H

#include <QList>
#include <QPoint>
#include <QString>
#include <QGesture>
#include <QGestureRecognizer>

#include "linjazaxgesture.h"

struct Direction
{
    QString direction;
    QPoint point;

    Direction(QString dir, const QPoint &pt)
        : direction(dir), point(pt) { }
    Direction()
        : direction() { }

    inline bool isEmpty() const { return direction.isEmpty(); }
    inline bool isNull() const { return direction.isEmpty(); }
};
typedef QList<Direction> DirectionList;

class DirectionSimpleRecognizer
{
public:
    DirectionSimpleRecognizer();
    Direction addPosition(const QPoint &pos);
    DirectionList getDirections() const;
    void reset();

private:
    QPoint lastPoint;
    DirectionList directions;
};

class GestureRecognizerLinjaZax : public QGestureRecognizer
{
    Q_OBJECT
public:
    GestureRecognizerLinjaZax();

    QGestureRecognizer::Result filterEvent(const QEvent *event);
    QGesture* getGesture();

    void reset();

private:
    void internalReset();

    QPoint pressedPos;
    QPoint lastPos;
    QPoint currentPos;
    bool mousePressed;
    bool gestureFinished;
    QString lastDirections;
    QString currentDirection;
    DirectionSimpleRecognizer simpleRecognizer;
    LinjaZaxGesture::ZoomState zoomState;
};

#endif
