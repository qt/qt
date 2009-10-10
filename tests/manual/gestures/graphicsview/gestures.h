#ifndef GESTURE_H
#define GESTURE_H

#include <QGestureRecognizer>
#include <QGesture>

class ThreeFingerSlideGesture : public QGesture
{
    Q_OBJECT
public:
    static Qt::GestureType Type;

    ThreeFingerSlideGesture(QObject *parent = 0) : QGesture(parent) { }

    bool gestureFired;
};

class ThreeFingerSlideGestureRecognizer : public QGestureRecognizer
{
private:
    QGesture* createGesture(QObject *target);
    QGestureRecognizer::Result filterEvent(QGesture *state, QObject *watched, QEvent *event);
    void reset(QGesture *state);
};

class RotateGestureRecognizer : public QGestureRecognizer
{
public:
    RotateGestureRecognizer();

private:
    QGesture* createGesture(QObject *target);
    QGestureRecognizer::Result filterEvent(QGesture *state, QObject *watched, QEvent *event);
    void reset(QGesture *state);
};

#endif // GESTURE_H
