#ifndef CUSTOMGESTURERECOGNIZER_H
#define CUSTOMGESTURERECOGNIZER_H

#include "qgesturerecognizer.h"
#include "qgesture.h"
#include "qevent.h"

class SingleshotEvent : public QEvent
{
public:
    static const int Type = QEvent::User + 1;

    QPoint point;

    explicit SingleshotEvent(int x = 0, int y = 0)
        : QEvent(QEvent::Type(Type)), point(x, y) { }
};

class SingleshotGesture : public QGesture
{
    Q_OBJECT
public:
    SingleshotGesture(QObject *parent, const QString &type)
        : QGesture(parent, type) { }

    QPoint offset;

protected:
    void translate(const QPoint &pt)
    {
        offset += pt;
    }
};

class SingleshotGestureRecognizer : public QGestureRecognizer
{
    Q_OBJECT
public:
    static const char *Name;

    SingleshotGestureRecognizer(QObject *parent = 0);

    QGestureRecognizer::Result filterEvent(const QEvent *event);
    QGesture* getGesture() { return gesture; }
    void reset();

private:
    SingleshotGesture *gesture;
};

struct TouchPoint {
    enum State
    {
        None = 0,
        Begin = Qt::TouchPointPressed,
        Update = Qt::TouchPointMoved,
        End = Qt::TouchPointReleased
    };
    int id;
    QPoint pt;
    State state;
    TouchPoint() : id(0), state(None) { }
    TouchPoint(int id_, int x_, int y_, State state_) : id(id_), pt(x_, y_), state(state_) { }
};

class TouchEvent : public QEvent
{
public:
    static const int Type = QEvent::User + 2;

    TouchEvent()
        : QEvent(QEvent::Type(Type))
    {
    }

    TouchPoint points[2];
};

class PinchGesture : public QGesture
{
    Q_OBJECT
public:
    PinchGesture(QObject *parent, const QString &type)
        : QGesture(parent, type) { }

    TouchPoint startPoints[2];
    TouchPoint lastPoints[2];
    TouchPoint points[2];

    QPoint offset;

protected:
    void translate(const QPoint &pt)
    {
        offset += pt;
    }
};

class PinchGestureRecognizer : public QGestureRecognizer
{
    Q_OBJECT
public:
    static const char *Name;

    PinchGestureRecognizer(QObject *parent = 0);

    QGestureRecognizer::Result filterEvent(const QEvent *event);
    QGesture* getGesture() { return gesture; }
    void reset();

private:
    PinchGesture *gesture;
};

class SecondFingerGesture : public QGesture
{
    Q_OBJECT
public:
    SecondFingerGesture(QObject *parent, const QString &type)
        : QGesture(parent, type) { }

    TouchPoint startPoint;
    TouchPoint lastPoint;
    TouchPoint point;

    QPoint offset;

protected:
    void translate(const QPoint &pt)
    {
        offset += pt;
    }
};

class SecondFingerGestureRecognizer : public QGestureRecognizer
{
    Q_OBJECT
public:
    static const char *Name;

    SecondFingerGestureRecognizer(QObject *parent = 0);

    QGestureRecognizer::Result filterEvent(const QEvent *event);
    QGesture* getGesture() { return gesture; }
    void reset();

private:
    SecondFingerGesture *gesture;
};

class PanGesture : public QGesture
{
    Q_OBJECT
public:
    PanGesture(QObject *parent, const QString &type)
        : QGesture(parent, type) { }

    TouchPoint startPoints[2];
    TouchPoint lastPoints[2];
    TouchPoint points[2];

    QPoint offset;

protected:
    void translate(const QPoint &pt)
    {
        offset += pt;
    }
};

class PanGestureRecognizer : public QGestureRecognizer
{
    Q_OBJECT
public:
    static const char *Name;

    PanGestureRecognizer(QObject *parent = 0);

    QGestureRecognizer::Result filterEvent(const QEvent *event);
    QGesture* getGesture() { return gesture; }
    void reset();

private:
    PanGesture *gesture;
};

#endif
